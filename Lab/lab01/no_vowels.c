#include <stdio.h>
#include <string.h>

int check_vowel(char word);

int main(void) {
    char str;
    while ((scanf("%c", &str)) != EOF){
        if (check_vowel(str) == 0) {
            printf("%c", str);
        }
    }
    return 0;  
}

int check_vowel(char word){
    if (word == 'a' || word == 'e' || word == 'i' || word == 'o' || word == 'u' ||word == 'A' || word == 'E' || word == 'I' || word == 'O' || word == 'U'){
        return 1;
    } 
    
    return 0;
}
