#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            printf("Hello Bob!\n");
        }

        #pragma omp section
        {
            printf("Hello Jane!\n");
        }

        #pragma omp section
        {
            printf("Hello Harry!\n");
        }

        #pragma omp section
        {
            printf("Hello Jud!\n");
        }
    }

    exit(EXIT_SUCCESS);
}
