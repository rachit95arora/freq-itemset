#!/bin/sh
### Reading Inputs
INPUT_FILE="$1"
SUPPORT="$2"
MODE="$3"
OUTPUT_FILE="$4"

if [[ "$SUPPORT" = "-plot" ]]; then
    MODE="-plot"
fi
case "${MODE}" in
    "-apriori")
        echo "[$(basename $0)] Generating frequent item set using Apriori algorithm"
        ./freq_itemset "$INPUT_FILE" "$OUTPUT_FILE" "$SUPPORT" 0
        ;;
    "-fptree")
        echo "[$(basename $0)] Generating frequent item set using FPTree algorithm"
        ./freq_itemset "$INPUT_FILE" "$OUTPUT_FILE" "$SUPPORT" 1
        ;;
    "-plot")
        echo "[$(basename $0)] Generating frequent item set using Apriori, FPTree algorithm and plotting relative performance"
        ;;
    *)
        echo "[$(basename $0)] Invalid mode option. Exiting"
        exit 1
esac

echo "[$(basename $0)] Run complete. Exiting"
