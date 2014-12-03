===============================================================================
    MGC Image Converter Tool %GESTIC_GUARD_VERSION_STRING%
===============================================================================

The MGC Image Conversion tool (MGCConv) allows extraction of firmware-images as C-structures from ENZ-Files.

Usage:

    mgcconv.exe <options> [<input> [<output>]]

Options:
    -h, --help
        show this help page
    -i, --input <file>
        select input file in the options
    -o, --output <file>
        select output file in  the options
    -c, --check
        validate the input file
    -w, --what <image>
        select whether library or loader image should be used
    -n, --name <varname>
        override name of variable in generated c-structure

Examples:
    Extracting C-Structure for library from input.enz:

        mgcconv.exe -w library input.enz -n Library library.c

    Checking input.enz for potential inconsistencies:

        mgcconv.exe -c input.enz
