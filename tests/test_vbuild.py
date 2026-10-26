"""Unit tests for VBuild; no compiler build or repository clone is needed."""
import importlib.util
import os
from pathlib import Path
import tempfile
import unittest
from unittest import mock

MODULE = Path(__file__).resolve().parents[1] / "tools" / "vbuild.py"
spec = importlib.util.spec_from_file_location("vbuild", MODULE)
vbuild = importlib.util.module_from_spec(spec)
spec.loader.exec_module(vbuild)


class VBuildTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix="vbuild project ")
        self.addCleanup(self.temp.cleanup)
        self.project = Path(self.temp.name)
        (self.project / "src").mkdir()
        (self.project / "src" / "main.vnt").write_text('print("hello")\n', encoding="utf-8")
        (self.project / "vbuild.toml").write_text(
            '[project]\nname = "space project"\nversion = "0.1.0"\n'
            '[build]\nsources = ["src/main.vnt"]\noutput = "space project"\n',
            encoding="utf-8",
        )

    def test_manifest_parses(self):
        manifest = vbuild.load_manifest(self.project)
        self.assertEqual(manifest["project"]["name"], "space project")
        self.assertEqual(manifest["build"]["sources"], ["src/main.vnt"])

    def test_missing_manifest_is_clear_error(self):
        (self.project / "vbuild.toml").unlink()
        with self.assertRaisesRegex(vbuild.VBuildError, "Missing vbuild.toml"):
            vbuild.load_manifest(self.project)

    def test_invalid_toml_is_rejected(self):
        (self.project / "vbuild.toml").write_text("[project\n", encoding="utf-8")
        with self.assertRaisesRegex(vbuild.VBuildError, "Cannot read"):
            vbuild.load_manifest(self.project)

    def test_missing_source_is_rejected(self):
        (self.project / "src" / "main.vnt").unlink()
        with self.assertRaisesRegex(vbuild.VBuildError, "Source file not found"):
            vbuild.source_paths(self.project, vbuild.load_manifest(self.project))

    def test_unsupported_target_is_rejected(self):
        (self.project / "vbuild.toml").write_text(
            '[project]\nname="demo"\n[build]\nsources=["src/main.vnt"]\ntarget="x86_64-linux"\n',
            encoding="utf-8",
        )
        with self.assertRaisesRegex(vbuild.VBuildError, "Only target"):
            vbuild.load_manifest(self.project)

    def test_build_invokes_existing_compiler_and_creates_output(self):
        manifest = vbuild.load_manifest(self.project)
        expected = vbuild.output_path(self.project, manifest)
        def fake_compile(args):
            self.assertEqual(args[0], "--compile")
            self.assertEqual(Path(args[1]), self.project / "src" / "main.vnt")
            self.assertEqual(args[2], "-o")
            Path(args[3]).parent.mkdir(parents=True, exist_ok=True)
            Path(args[3]).write_text("fake executable", encoding="utf-8")
        with mock.patch.object(vbuild, "invoke_compiler", side_effect=fake_compile):
            old = Path.cwd()
            os.chdir(self.project)
            try:
                self.assertEqual(vbuild.main(["build"]), 0)
            finally:
                os.chdir(old)
        self.assertTrue(expected.is_file())

    def test_compiler_failure_is_reported(self):
        old = Path.cwd()
        os.chdir(self.project)
        try:
            with mock.patch.object(vbuild, "invoke_compiler", side_effect=vbuild.VBuildError("VNT compiler failed with exit code 7.")):
                self.assertEqual(vbuild.main(["build"]), 1)
        finally:
            os.chdir(old)

    def test_clean_removes_only_build_directory(self):
        source = self.project / "src" / "main.vnt"
        generated = self.project / "build" / "generated"
        generated.parent.mkdir()
        generated.write_text("generated", encoding="utf-8")
        old = Path.cwd()
        os.chdir(self.project)
        try:
            self.assertEqual(vbuild.main(["clean"]), 0)
        finally:
            os.chdir(old)
        self.assertTrue(source.is_file())
        self.assertFalse((self.project / "build").exists())

    def test_init_refuses_to_overwrite_manifest(self):
        old = Path.cwd()
        os.chdir(self.project)
        try:
            with self.assertRaisesRegex(vbuild.VBuildError, "already exists"):
                vbuild.command_init(self.project, None)
        finally:
            os.chdir(old)


if __name__ == "__main__":
    unittest.main()
