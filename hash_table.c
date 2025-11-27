#include <stdio.h>
#include "hash_table_without_free_bit.h"

int main() {
    // Intersting to note that without the \n at the end, I got an output of: "Maybe Hash Table!!%".
    // The trailing % at the end is a  zsh thing, it indicates that we didn't end in a new line.
    // :woozy_face:
    printf("Maybe Hash Table!!\n");
    printf("DEAFULT_KEY: %u\n", DEFAULT_KEY);

    return 0;
}
