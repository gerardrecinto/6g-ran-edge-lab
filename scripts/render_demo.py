#!/usr/bin/env python3
"""Render a terminal-style animated GIF from the simulator output."""

from __future__ import annotations

import html
import subprocess
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "docs" / "assets" / "demo.gif"


def main() -> None:
    build = ROOT / "build"
    subprocess.run(["make", "-C", str(ROOT)], check=True, stdout=subprocess.DEVNULL)
    output = subprocess.check_output([str(build / "ranedge-sim"), "--ticks", "10"], text=True)

    try:
        from PIL import Image, ImageDraw, ImageFont
    except ImportError as exc:
        raise SystemExit("Pillow is required to render demo.gif. Install with: python3 -m pip install Pillow") from exc

    width, height = 1120, 560
    bg = (8, 12, 18)
    shell = (18, 26, 36)
    green = (88, 255, 167)
    blue = (98, 177, 255)
    red = (255, 108, 108)
    text = (224, 234, 244)
    muted = (127, 145, 163)

    font_paths = [
        "/System/Library/Fonts/Menlo.ttc",
        "/Library/Fonts/Menlo.ttc",
        "/System/Library/Fonts/SFNSMono.ttf",
    ]
    font = None
    title_font = None
    for path in font_paths:
        if Path(path).exists():
            font = ImageFont.truetype(path, 20)
            title_font = ImageFont.truetype(path, 22)
            break
    if font is None:
        font = ImageFont.load_default()
        title_font = font

    lines = ["$ ranedge-sim --ticks 10"] + output.rstrip().splitlines()
    frames = []
    for visible in range(2, len(lines) + 1):
        image = Image.new("RGB", (width, height), bg)
        draw = ImageDraw.Draw(image)
        draw.rounded_rectangle((28, 28, width - 28, height - 28), radius=10, fill=shell, outline=(42, 58, 75), width=2)
        draw.ellipse((50, 48, 66, 64), fill=red)
        draw.ellipse((74, 48, 90, 64), fill=(255, 204, 92))
        draw.ellipse((98, 48, 114, 64), fill=(87, 214, 139))
        draw.text((140, 45), "6g-ran-edge-lab / C++ RAN edge simulator", font=title_font, fill=muted)

        y = 92
        for idx, line in enumerate(lines[:visible]):
            fill = green if idx == 0 else text
            if "latency-regression" in line or "edge-cpu-pressure" in line:
                fill = blue
            draw.text((54, y), html.unescape(line), font=font, fill=fill)
            y += 30

        frames.append(image)

    OUT.parent.mkdir(parents=True, exist_ok=True)
    frames[0].save(OUT, save_all=True, append_images=frames[1:], duration=240, loop=0, optimize=True)
    print(OUT)


if __name__ == "__main__":
    main()
