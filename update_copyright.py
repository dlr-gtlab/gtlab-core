# SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

import os
import re
import pathlib
import datetime
import argparse


def change_year(current_year, folder):

    print("Changing copyright year to %i in folder %s" % (current_year, folder))
    files = os.walk(folder)
    pattern = re.compile(r"(SPDX-FileCopyrightText:)\s*(\d{4})\s*(German Aerospace Center\s*\(DLR\))")
    about_dialog_pattern = re.compile(r"(copyright)\s*(\d{4})\s*(by DLR)")
    file_endings = [".txt", ".md", ".cpp", ".h", ".license"]

    for dirpath, dirnames, filenames in files:
        for filename in filenames:

            if not pathlib.Path(filename).suffix in file_endings:
                continue

            file = open(os.path.join(dirpath, filename), 'r')
            file_content = file.read()
            file.close()

            fixed_file_content = pattern.sub("SPDX-FileCopyrightText: "
                                             + str(current_year) + " German Aerospace Center (DLR)", file_content)

            fixed_file_content = about_dialog_pattern.sub("copyright "
                                                          + str(current_year) + " by DLR", fixed_file_content)

            file = open(os.path.join(dirpath, filename), 'w')
            file.write(fixed_file_content)
            file.close()


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument("-y", "--year",
                        help="Replaces the year of the copyright licenses with the current year",
                        nargs='?', const=datetime.date.today().year, default=datetime.date.today().year, type=int)
    parser.add_argument('directory')

    args = parser.parse_args()

    change_year(current_year=args.year, folder=args.directory)


if __name__ == "__main__":
    main()
