import csv
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt


folder = Path(__file__).resolve().parent
with (folder / "7-3-timing.csv").open(newline="", encoding="utf-8") as stream:
    rows = list(csv.DictReader(stream))

fig, axes = plt.subplots(2, 2, figsize=(8.5, 5.9), sharex=True)
for ax, m in zip(axes.flat, (15, 20, 25, 30)):
    series = sorted((row for row in rows if int(row["M"]) == m),
                    key=lambda row: int(row["N"]))
    x = [int(row["N"]) / 10**6 for row in series]
    for field, label, color, marker, style in [
        ("brute_b_ms", "Brute force B", "#134e76", "o", "-"),
        ("kmp_ms", "Optimized Next KMP", "#b7472a", "s", "--"),
    ]:
        ax.plot(x, [float(row[field]) for row in series], style,
                color=color, marker=marker, linewidth=1.7, markersize=4,
                label=label)
    ax.set_title(f"M = {m}", loc="left", fontsize=10.5, fontweight="bold")
    ax.set_ylim(bottom=0)
    ax.set_xticks(x, [f"{length:.2f}" for length in x])
    ax.grid(axis="y", color="#dce5ec", linewidth=0.8)
    ax.spines[["top", "right"]].set_visible(False)

fig.subplots_adjust(left=0.11, right=0.98, top=0.88, bottom=0.12,
                    wspace=0.23, hspace=0.31)
fig.legend(*axes[0, 0].get_legend_handles_labels(), loc="upper center",
           ncol=2, frameon=False, bbox_to_anchor=(0.54, 0.99))
fig.supxlabel("Text length N (million characters)", fontsize=10)
fig.supylabel("Time per batch of 200 searches (ms)", fontsize=10)
fig.savefig(folder / "7-3-timing.png", dpi=200, facecolor="white")
