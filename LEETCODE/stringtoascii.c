#include <stdio.h>
#include <string.h>

int main() 
{
    int t;
    scanf("%d", &t);

    while(t--)
    {
        char S[100];
        scanf("%s", S);
        int A[strlen(S)];

        for(int i = 0; i < strlen(S); i++)
        {
            //Converts a character into its ASCII value
            A[i] = (int)S[i];
        }
        for(int i = 0; i < strlen(S); i++)
        {
            printf("%d ", A[i]);
        }
        printf("\n");    
    }
    return 0;
}
