#!/bin/bash

# Loop through all .o files in the current directory
for obj_file in *.o; do
    # Extract the base name (without the .o extension)
    base_name="${obj_file%.o}"

    # Create the static library with the same name as the object file
    echo "Creating library for $obj_file..."
    ar rcs "lib${base_name}.a" "$obj_file"

    # Verify the library contents
    echo "Library contents of lib${base_name}.a:"
    ar t "lib${base_name}.a"
done

echo "All libraries created successfully!"
