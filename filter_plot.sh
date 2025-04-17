#!/usr/bin/env bash
set -euo pipefail

# ────────────────────────────────────────────────────────────
# 1) CONFIGURATION ─ change these to point at your data/cols
# ────────────────────────────────────────────────────────────
DATA1_FILE="Output-1stOrder/Temperature_Cycle/Roots_1.dat"
DATA1_SKIP=1 #skip row
DATA1_XCOL=21
DATA1_YCOL=1
DATA1_TITLE="data1.dat (Reference)"
DATA1_COLOR="blue"

DATA2_FILE="Output-Quark/Temperature_Cycle/Roots_7.dat"
DATA2_SKIP=1 #skip row in file
DATA2_XCOL=13
DATA2_YCOL=14
DATA2_TITLE="data2.dat (Modifiable)"
DATA2_COLOR="red"

REGION_FILE="selected_region.txt"
BACKUP_FILE="${DATA2_FILE}.bak"
# ────────────────────────────────────────────────────────────

# 2) UNDO if necessary
if [ "${1:-}" = "undo" ]; then
  if [ -f "$BACKUP_FILE" ]; then
    mv "$BACKUP_FILE" "$DATA2_FILE"
    echo "Restored original: $DATA2_FILE ← $BACKUP_FILE"
    exit 0
  else
    echo "No backup ($BACKUP_FILE) found; cannot undo."
    exit 1
  fi
fi

# 3) Generate a temporary Gnuplot script
GNUPLOT_SCRIPT=$(mktemp /tmp/plot_XXXX.gp)
cat > "$GNUPLOT_SCRIPT" <<-EOF
  set terminal x11 persist
  set mouse
  set grid
  set xlabel "X-axis"
  set ylabel "Y-axis"
  set title  "Data Visualization & Region Selection"

  plot \
    "$DATA1_FILE" every ::$DATA1_SKIP using $DATA1_XCOL:$DATA1_YCOL \
      with points title "$DATA1_TITLE" lc rgb "$DATA1_COLOR", \
    "$DATA2_FILE" every ::$DATA2_SKIP using $DATA2_XCOL:$DATA2_YCOL \
      with points title "$DATA2_TITLE" lc rgb "$DATA2_COLOR"

  print "Zoom/Pan freely, then click bottom-left and top-right of the region."
  pause -1 "Press Enter when ready to select the region"

  pause mouse
  x_min = MOUSE_X;  y_min = MOUSE_Y
  print sprintf("Bottom-left: (%f, %f)", x_min, y_min)

  pause mouse
  x_max = MOUSE_X;  y_max = MOUSE_Y
  print sprintf("Top-right: (%f, %f)", x_max, y_max)

  if (x_min > x_max) { tmp = x_min; x_min = x_max; x_max = tmp }
  if (y_min > y_max) { tmp = y_min; y_min = y_max; y_max = tmp }

  set object 1 rectangle from x_min,y_min to x_max,y_max \
       fc rgb "gray" fs transparent solid 0.2 border lc rgb "black"
  replot

  set print "$REGION_FILE"
  print x_min, x_max, y_min, y_max
  set print

  print sprintf("Selected region saved: x=[%f, %f], y=[%f, %f]", \
                x_min, x_max, y_min, y_max)
EOF

# 4) Run gnuplot (this will block until  finished clicks)
echo "Launching Gnuplot..."
gnuplot "$GNUPLOT_SCRIPT"
rm "$GNUPLOT_SCRIPT"

if [ ! -f "$REGION_FILE" ]; then
  echo "Error: selection not saved. No $REGION_FILE found."
  exit 1
fi

read -r x_min x_max y_min y_max < "$REGION_FILE"
echo "Region: x_min=$x_min, x_max=$x_max, y_min=$y_min, y_max=$y_max"

# Reorder if needed
if (( $(echo "$x_min > $x_max" | bc -l) )); then
  tmp=$x_min; x_min=$x_max; x_max=$tmp
fi
if (( $(echo "$y_min > $y_max" | bc -l) )); then
  tmp=$y_min; y_min=$y_max; y_max=$tmp
fi
echo "After ordering: x_min=$x_min, x_max=$x_max, y_min=$y_min, y_max=$y_max"

# 5) Backup original once
if [ ! -f "$BACKUP_FILE" ]; then
  cp "$DATA2_FILE" "$BACKUP_FILE"
  echo "Created backup: $BACKUP_FILE"
fi

# 6) Filter out points inside the selected box
tmpf=$(mktemp)
awk -v xcol="$DATA2_XCOL" -v ycol="$DATA2_YCOL" \
    -v xmin="$x_min" -v xmax="$x_max" \
    -v ymin="$y_min" -v ymax="$y_max" '
{
  x = $xcol; y = $ycol;
  # keep only points outside the box
  if (x < xmin || x > xmax || y < ymin || y > ymax) print
}' "$DATA2_FILE" > "$tmpf"

mv "$tmpf" "$DATA2_FILE"
echo "Filtered data saved to $DATA2_FILE"
echo "To undo, run: ./$(basename "$0") undo"

# Remove temp. file for selecting area 
rm selected_region.txt
