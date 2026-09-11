#!/usr/bin/env python3
"""Build the Khicas Opt calculator package with the shared Linux/WSL2 script."""

from __future__ import annotations

import argparse
import os
from pathlib import Path
import subprocess


ROOT = Path(__file__).resolve().parents[1]


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "profile",
        choices=("optimized",),
        nargs="?",
        default="optimized",
        help="Build profile. The repository uses the optimized Khicas Opt profile.",
    )
    parser.add_argument(
        "--tools-dir",
        type=Path,
        default=None,
        help="Toolchain directory passed to build.sh through TOOLS_DIR.",
    )
    parser.add_argument("-j", "--jobs", type=int, default=None)
    args = parser.parse_args()

    env = dict(os.environ)
    if args.tools_dir is not None:
        env["TOOLS_DIR"] = str(args.tools_dir.resolve())
    if args.jobs is not None:
        env["JOBS"] = str(args.jobs)

    subprocess.run(["bash", str(ROOT / "build.sh")], cwd=ROOT, env=env, check=True)


if __name__ == "__main__":
    main()
