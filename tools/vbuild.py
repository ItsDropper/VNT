#!/usr/bin/env python3
"""VBuild: lightweight project driver for the VNT compiler (Python 3.11+)."""
from __future__ import annotations

import argparse
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tomllib


MANIFEST = "vbuild.toml"
TOOLCHAIN_ROOT = Path(__file__).resolve().parent.parent


class VBuildError(Exception):
    pass


def load_manifest(project: Path) -> dict:
    path = project / MANIFEST
    if not path.is_file():
        raise VBuildError(f"Missing {MANIFEST}; run 'vbuild init' first.")
    try:
        with path.open("rb") as stream:
            data = tomllib.load(stream)
    except (tomllib.TOMLDecodeError, OSError) as exc:
        raise VBuildError(f"Cannot read {MANIFEST}: {exc}") from exc

    section = data.get("project")
    if not isinstance(section, dict):
        raise VBuildError(f"{MANIFEST}: [project] table is required.")
    name = section.get("name")
    version = section.get("version", "0.1.0")
    if not isinstance(name, str) or not name.strip():
        raise VBuildError(f"{MANIFEST}: project.name must be a non-empty string.")
    if not isinstance(version, str) or not version.strip():
        raise VBuildError(f"{MANIFEST}: project.version must be a non-empty string.")

    build = data.get("build", {})
    if not isinstance(build, dict):
        raise VBuildError(f"{MANIFEST}: [build] must be a table.")
    sources = build.get("sources", ["src/main.vnt"])
    if not isinstance(sources, list) or not sources or any(
        not isinstance(item, str) or not item.strip() for item in sources
    ):
        raise VBuildError(f"{MANIFEST}: build.sources must be a non-empty array of paths.")
    if len(sources) != 1:
        raise VBuildError("This VBuild version accepts one root source; use VNT imports for additional modules.")
    output = build.get("output", name)
    if not isinstance(output, str) or not output.strip() or Path(output).name != output:
        raise VBuildError(f"{MANIFEST}: build.output must be a file name, not a path.")
    target = build.get("target", "host")
    if target != "host":
        raise VBuildError("Only target = \"host\" is supported by this VBuild version.")
    return {"project": section, "build": {**build, "sources": sources, "output": output}}


def compiler_path() -> Path:
    override = os.environ.get("VNT_COMPILER")
    if override:
        path = Path(override).expanduser().resolve()
        if path.is_file():
            return path
        raise VBuildError(f"VNT_COMPILER points to a missing file: {path}")
    candidates = [TOOLCHAIN_ROOT / ("vnt.exe" if os.name == "nt" else "vnt"),
                  TOOLCHAIN_ROOT / "vnt.exe", TOOLCHAIN_ROOT / "vnt"]
    for candidate in candidates:
        if candidate.is_file():
            return candidate
    raise VBuildError(
        f"VNT compiler not found in {TOOLCHAIN_ROOT}. Build VNT first or set VNT_COMPILER."
    )


def source_paths(project: Path, manifest: dict) -> list[Path]:
    paths = []
    for item in manifest["build"]["sources"]:
        source = (project / item).resolve()
        try:
            source.relative_to(project.resolve())
        except ValueError as exc:
            raise VBuildError(f"Source path escapes the project directory: {item}") from exc
        if not source.is_file():
            raise VBuildError(f"Source file not found: {item}")
        paths.append(source)
    return paths


def output_path(project: Path, manifest: dict) -> Path:
    suffix = ".exe" if os.name == "nt" else ""
    return (project / "build" / (manifest["build"]["output"] + suffix)).resolve()


def invoke_compiler(args: list[str]) -> None:
    compiler = compiler_path()
    # The current compiler links runtime sources using paths relative to its repository.
    # Run it from the toolchain root while passing absolute project paths.
    result = subprocess.run([str(compiler), *args], cwd=TOOLCHAIN_ROOT, check=False)
    if result.returncode:
        raise VBuildError(f"VNT compiler failed with exit code {result.returncode}.")


def command_init(project: Path, name: str | None) -> None:
    project.mkdir(parents=True, exist_ok=True)
    manifest = project / MANIFEST
    if manifest.exists():
        raise VBuildError(f"{MANIFEST} already exists; refusing to overwrite it.")
    project_name = name or project.name or "vnt-project"
    if not project_name.strip() or any(c in project_name for c in '\r\n"'):
        raise VBuildError("Project name must be a non-empty, single-line string.")
    manifest.write_text(
        f'[project]\nname = "{project_name}"\nversion = "0.1.0"\n\n'
        '[build]\nsources = ["src/main.vnt"]\noutput = "' + project_name +
        '"\ntarget = "host"\n',
        encoding="utf-8",
    )
    source = project / "src" / "main.vnt"
    source.parent.mkdir(parents=True, exist_ok=True)
    source.write_text('print("Hello from VNT")\n', encoding="utf-8")
    print(f"Initialized VNT project: {project_name}")


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(prog="vbuild", description="Build native VNT projects.")
    sub = parser.add_subparsers(dest="command", required=True)
    init = sub.add_parser("init", help="create a project manifest and starter source")
    init.add_argument("--name")
    for name, help_text in (
        ("build", "compile project sources"),
        ("run", "build and run the project"),
        ("check", "parse, lower, and validate project sources without linking"),
        ("clean", "remove generated files in the build directory"),
        ("test", "reserved for manifest-defined test targets; not implemented yet"),
    ):
        sub.add_parser(name, help=help_text)

    args = parser.parse_args(argv)
    project = Path.cwd().resolve()
    try:
        if args.command == "init":
            command_init(project, args.name)
            return 0

        manifest = load_manifest(project)
        if args.command == "test":
            raise VBuildError("VBuild test targets are not implemented yet; no tests were run.")
        build_dir = (project / "build").resolve()
        try:
            build_dir.relative_to(project)
        except ValueError as exc:
            raise VBuildError("Build directory must remain inside the project.") from exc

        if args.command == "clean":
            if build_dir.exists():
                shutil.rmtree(build_dir)
            print("Cleaned build/")
            return 0

        sources = source_paths(project, manifest)
        if args.command == "check":
            for source in sources:
                invoke_compiler(["--dump-ir", str(source)])
            print("Check successful.")
            return 0

        build_dir.mkdir(parents=True, exist_ok=True)
        executable = output_path(project, manifest)
        invoke_compiler(["--compile", str(sources[0]), "-o", str(executable)])
        if args.command == "build":
            print(f"Build successful: {executable}")
            return 0
        if args.command == "run":
            if not executable.is_file():
                raise VBuildError(f"Compiler reported success but output is missing: {executable}")
            result = subprocess.run([str(executable)], cwd=project, check=False)
            return result.returncode
        return 0
    except VBuildError as exc:
        print(f"vbuild: error: {exc}", file=sys.stderr)
        return 1
    except OSError as exc:
        print(f"vbuild: error: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
