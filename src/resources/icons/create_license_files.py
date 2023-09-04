import os

# Define the directory where your .svg files are located
directory_path = '.'

file_filter = '.svg'            

license_text = """
SPDX-License-Identifier: CC
SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
"""
license_text_2 = """
SPDX-License-Identifier: Apache-2.0
SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
"""

# Iterate through all files in the directory
for filename in os.listdir(directory_path):
    if filename.endswith(file_filter):
        # Get the file suffix
        file_suffix = os.path.splitext(filename)[1]

        # Determine the license text based on filename
        text = license_text
        if not filename.endswith('_custom.svg'):
            text = license_text_2
            
        # Create the new file with the desired content
        new_filename = os.path.splitext(filename)[0] + file_suffix + '.license'
        new_file_path = os.path.join(directory_path, new_filename)

        with open(new_file_path, 'w') as new_file:
            new_file.write(text)

        print(f"Created {new_filename}")