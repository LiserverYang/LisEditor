#include "config.h"

Configtion config = Configtion();

void parse_args(int argc, const char **argv, Configtion &config) noexcept
{
    for (int i = 1; i < argc; i++)
    {
        // arg
        if (argv[i][0] == '-')
        {
            if (0 == strcmp(argv[i], "-ro"))
            {
                config._readonly = true;
            }
            else if (0 == strcmp(argv[i], "-readonly"))
            {
                config._readonly = true;
            }
            else if (0 == strcmp(argv[i], "-create"))
            {
                config._create = true;
            }
        }
        else // file
        {
            config._path = argv[i];
        }
    }
}