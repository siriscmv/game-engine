#!/bin/bash

# Specify the directory containing the JSON files
DIRECTORY="." # Change this to your directory path
OUTPUT_FILE="merged.json"

# Start the merged JSON file
echo '{"results": [' > "$OUTPUT_FILE"

# Initialize a flag to handle comma placement
first=true

# Iterate over all JSON files in the directory
for file in "$DIRECTORY"/*.json; do
    # Check if there are any JSON files
    [ -e "$file" ] || continue  # Skip if no JSON files are found

    # Extract the first element from the results array and format it
    if $first; then
        # First entry, no leading comma
        jq '.results[0]' "$file" >> "$OUTPUT_FILE"
        first=false
    else
        # Subsequent entries, prepend a comma
        echo "," >> "$OUTPUT_FILE"
        jq '.results[0]' "$file" >> "$OUTPUT_FILE"
    fi
done

# Close the results array and the JSON object
echo ']}' >> "$OUTPUT_FILE"

echo "Merged JSON file created: $OUTPUT_FILE"
