# Tutorial 5: A First Windows GUI

VNT includes a basic native GUI API for Windows. This tutorial starts from the repository's working GUI example rather than implying that the GUI layer is cross-platform or a complete widget toolkit.

## 1. Inspect the example

Open [`examples/gui_demo.vnt`](../../examples/gui_demo.vnt) and [`examples/gui_demo.css`](../../examples/gui_demo.css). The source demonstrates the GUI lifecycle and basic drawing APIs:

- `gui_css(path)` loads the supported CSS-like stylesheet.
- `gui_size(width, height)` configures the window size.
- `gui_open(title)` creates a window.
- `gui_text(text)` draws a line of text.
- `gui_button(label, x, y)` draws a button and reports clicks.
- `gui_poll()` processes window events.
- `gui_close()` closes the window.

The API also includes drawing and input helpers; inspect the example and runtime implementation for current signatures and limitations.

## 2. Compile and run

Run these commands from the repository root on Windows:

```powershell
.\vnt.exe --compile examples\gui_demo.vnt -o gui_demo.exe
.\gui_demo.exe
```

The stylesheet path in the example is relative to the process's working directory, so launching from the repository root matters.

## 3. Try a small customization

Edit `examples/gui_demo.css` and change a property already used by the file, such as a color or font size. Run the application again to see the effect. Only the documented CSS subset is supported; this is not a browser engine, and unsupported CSS properties are ignored.

## Important limitations

- GUI support currently targets Windows.
- The API is a prototype foundation, not a complete accessibility-ready widget toolkit.
- The CSS-like styling supports a small set of selectors and properties.
- Input handling has limitations, including basic character input rather than full international text input.

For a larger application, inspect [`examples/vnt_focus.vnt`](../../examples/vnt_focus.vnt) and [`examples/vanta_one.vnt`](../../examples/vanta_one.vnt). Those examples are more substantial than the minimal GUI demo.
