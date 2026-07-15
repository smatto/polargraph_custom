#!/usr/bin/env python3
"""Convert SVG vector shapes into Polargraph draw commands in drawing.h format."""

import argparse
import math
import re
import xml.etree.ElementTree as ET
from typing import Iterable, Iterator, List, Tuple

FLOAT_RE = re.compile(r"[-+]?(?:\d*\.\d+|\d+)(?:[eE][-+]?\d+)?")
COMMAND_RE = re.compile(r"([MmZzLlHhVvCcSsQqTtAa])")


def parse_floats(text: str) -> List[float]:
    return [float(num) for num in FLOAT_RE.findall(text)]


class SvgPathTokenizer:
    def __init__(self, pathdata: str):
        self.tokens = COMMAND_RE.split(pathdata)
        self.tokens = [tok.strip() for tok in self.tokens if tok.strip()]
        self.index = 0

    def __iter__(self) -> Iterator[Tuple[str, str]]:
        while self.index < len(self.tokens):
            cmd = self.tokens[self.index]
            self.index += 1
            if self.index >= len(self.tokens):
                args = ""
            else:
                args = self.tokens[self.index]
                self.index += 1
            yield cmd, args


def sample_cubic(p0, p1, p2, p3, steps=20):
    points = []
    for i in range(1, steps + 1):
        t = i / steps
        a = (1 - t) ** 3
        b = 3 * t * (1 - t) ** 2
        c = 3 * (t ** 2) * (1 - t)
        d = t ** 3
        x = a * p0[0] + b * p1[0] + c * p2[0] + d * p3[0]
        y = a * p0[1] + b * p1[1] + c * p2[1] + d * p3[1]
        points.append((x, y))
    return points


def sample_quadratic(p0, p1, p2, steps=20):
    points = []
    for i in range(1, steps + 1):
        t = i / steps
        a = (1 - t) ** 2
        b = 2 * (1 - t) * t
        c = t ** 2
        x = a * p0[0] + b * p1[0] + c * p2[0]
        y = a * p0[1] + b * p1[1] + c * p2[1]
        points.append((x, y))
    return points


def arc_to_cubic(start, rx, ry, x_axis_rotation, large_arc_flag, sweep_flag, end):
    # For now, approximate arcs as straight line to end.
    return [end]


def flatten_path_d(pathdata: str) -> List[List[Tuple[float, float]]]:
    result = []
    current = (0.0, 0.0)
    start_point = (0.0, 0.0)
    control = None
    current_subpath: List[Tuple[float, float]] = []

    for cmd, argstr in SvgPathTokenizer(pathdata):
        numbers = parse_floats(argstr)
        idx = 0
        while idx < len(numbers) or cmd.upper() in "Z":
            if cmd == "M":
                x, y = numbers[idx], numbers[idx + 1]
                current = (x, y)
                start_point = current
                current_subpath = [current]
                result.append(current_subpath)
                idx += 2
                if idx < len(numbers):
                    cmd = "L"
                continue
            if cmd == "m":
                x, y = numbers[idx], numbers[idx + 1]
                current = (current[0] + x, current[1] + y)
                start_point = current
                current_subpath = [current]
                result.append(current_subpath)
                idx += 2
                if idx < len(numbers):
                    cmd = "l"
                continue
            if cmd == "L":
                x, y = numbers[idx], numbers[idx + 1]
                current = (x, y)
                current_subpath.append(current)
                idx += 2
                continue
            if cmd == "l":
                x, y = numbers[idx], numbers[idx + 1]
                current = (current[0] + x, current[1] + y)
                current_subpath.append(current)
                idx += 2
                continue
            if cmd == "H":
                x = numbers[idx]
                current = (x, current[1])
                current_subpath.append(current)
                idx += 1
                continue
            if cmd == "h":
                x = numbers[idx]
                current = (current[0] + x, current[1])
                current_subpath.append(current)
                idx += 1
                continue
            if cmd == "V":
                y = numbers[idx]
                current = (current[0], y)
                current_subpath.append(current)
                idx += 1
                continue
            if cmd == "v":
                y = numbers[idx]
                current = (current[0], current[1] + y)
                current_subpath.append(current)
                idx += 1
                continue
            if cmd == "C":
                p1 = (numbers[idx], numbers[idx + 1])
                p2 = (numbers[idx + 2], numbers[idx + 3])
                p3 = (numbers[idx + 4], numbers[idx + 5])
                samples = sample_cubic(current, p1, p2, p3)
                current_subpath.extend(samples)
                current = p3
                control = p2
                idx += 6
                continue
            if cmd == "c":
                p1 = (current[0] + numbers[idx], current[1] + numbers[idx + 1])
                p2 = (current[0] + numbers[idx + 2], current[1] + numbers[idx + 3])
                p3 = (current[0] + numbers[idx + 4], current[1] + numbers[idx + 5])
                samples = sample_cubic(current, p1, p2, p3)
                current_subpath.extend(samples)
                current = p3
                control = p2
                idx += 6
                continue
            if cmd == "Q":
                p1 = (numbers[idx], numbers[idx + 1])
                p2 = (numbers[idx + 2], numbers[idx + 3])
                samples = sample_quadratic(current, p1, p2)
                current_subpath.extend(samples)
                current = p2
                control = p1
                idx += 4
                continue
            if cmd == "q":
                p1 = (current[0] + numbers[idx], current[1] + numbers[idx + 1])
                p2 = (current[0] + numbers[idx + 2], current[1] + numbers[idx + 3])
                samples = sample_quadratic(current, p1, p2)
                current_subpath.extend(samples)
                current = p2
                control = p1
                idx += 4
                continue
            if cmd == "Z" or cmd == "z":
                if current_subpath and current_subpath[-1] != start_point:
                    current_subpath.append(start_point)
                current = start_point
                idx += 0
                break
            print(f"Warning: unsupported path command {cmd}")
            break
    return [sub for sub in result if len(sub) >= 2]


def parse_points(text: str) -> List[Tuple[float, float]]:
    coords = parse_floats(text)
    return [(coords[i], coords[i + 1]) for i in range(0, len(coords), 2)]


def apply_transform(point, scale: float, offset_x: float, offset_y: float, flip_y: bool) -> Tuple[float, float]:
    x = point[0] * scale + offset_x
    y = point[1] * scale + offset_y
    return (x, -y) if flip_y else (x, y)


def extract_paths(svg_root: ET.Element) -> List[List[Tuple[float, float]]]:
    paths: List[List[Tuple[float, float]]] = []
    ns = {"svg": "http://www.w3.org/2000/svg"}

    for element in svg_root.iter():
        tag = element.tag.split("}")[-1]
        if tag == "path":
            d = element.get("d", "")
            if d.strip():
                paths.extend(flatten_path_d(d))
        elif tag == "polyline":
            points = element.get("points", "")
            pts = parse_points(points)
            if pts:
                paths.append(pts)
        elif tag == "polygon":
            points = element.get("points", "")
            pts = parse_points(points)
            if pts:
                if pts[0] != pts[-1]:
                    pts.append(pts[0])
                paths.append(pts)
        elif tag == "line":
            x1 = float(element.get("x1", "0"))
            y1 = float(element.get("y1", "0"))
            x2 = float(element.get("x2", "0"))
            y2 = float(element.get("y2", "0"))
            paths.append([(x1, y1), (x2, y2)])
        elif tag == "rect":
            x = float(element.get("x", "0"))
            y = float(element.get("y", "0"))
            width = float(element.get("width", "0"))
            height = float(element.get("height", "0"))
            pts = [(x, y), (x + width, y), (x + width, y + height), (x, y + height), (x, y)]
            paths.append(pts)
        elif tag == "circle":
            cx = float(element.get("cx", "0"))
            cy = float(element.get("cy", "0"))
            r = float(element.get("r", "0"))
            pts = []
            segments = 32
            for i in range(segments + 1):
                theta = 2 * math.pi * i / segments
                pts.append((cx + r * math.cos(theta), cy + r * math.sin(theta)))
            paths.append(pts)
    return paths


def format_commands(paths: List[List[Tuple[float, float]]], scale: float, offset_x: float, offset_y: float, flip_y: bool) -> str:
    output_lines = ["// Generated by svg_to_drawing_h.py\n"]
    for path in paths:
        start = apply_transform(path[0], scale, offset_x, offset_y, flip_y)
        output_lines.append(f"startPath({start[0]:.2f}, {start[1]:.2f});")
        for pt in path[1:]:
            p = apply_transform(pt, scale, offset_x, offset_y, flip_y)
            output_lines.append(f"drawTo({p[0]:.2f}, {p[1]:.2f});")
        output_lines.append("endPath();\n")
    return "\n".join(output_lines)


def main() -> None:
    parser = argparse.ArgumentParser(description="Convert SVG to polargraph drawing.h commands.")
    parser.add_argument("svg_file", help="Input SVG file")
    parser.add_argument("output", help="Output drawing.h file")
    parser.add_argument("--scale", type=float, default=1.0, help="Scale factor from SVG units to machine mm")
    parser.add_argument("--offset-x", type=float, default=0.0, help="X offset in machine coordinates")
    parser.add_argument("--offset-y", type=float, default=0.0, help="Y offset in machine coordinates")
    parser.add_argument("--flip-y", action="store_true", help="Flip the SVG Y axis")
    args = parser.parse_args()

    tree = ET.parse(args.svg_file)
    root = tree.getroot()
    paths = extract_paths(root)
    if not paths:
        raise SystemExit("No supported paths found in SVG.")

    content = format_commands(paths, args.scale, args.offset_x, args.offset_y, args.flip_y)
    with open(args.output, "w", encoding="utf-8") as f:
        f.write(content)
    print(f"Wrote {len(paths)} paths to {args.output}")


if __name__ == "__main__":
    main()
