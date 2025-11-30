#include "random_array_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h> // Để sử dụng size_t
#include <time.h>   // Để sử dụng time()
// Hàm hoán đổi hai phần tử trong mảng
void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Hàm lấy ngẫu nhiên m phần tử không trùng từ mảng đầu vào
int *getRandomSubset(const int *arr, size_t size, size_t m)
{
    // Kiểm tra điều kiện hợp lệ
    if (m > size || arr == NULL)
    {
        fprintf(stderr, "Error: Invalid input (m > size or NULL array)\n");
        return NULL;
    }

    // Khởi tạo bộ sinh số ngẫu nhiên
    srand((unsigned int)time(NULL));

    // Sao chép mảng đầu vào để tránh thay đổi dữ liệu gốc
    int *temp = (int *)malloc(size * sizeof(int));
    if (temp == NULL)
    {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }
    for (size_t i = 0; i < size; i++)
    {
        temp[i] = arr[i];
    }

    // Fisher-Yates Shuffle để trộn ngẫu nhiên
    for (size_t i = size - 1; i > 0; i--)
    {
        size_t j = rand() % (i + 1);
        swap(&temp[i], &temp[j]);
    }

    // Cấp phát bộ nhớ cho kết quả và sao chép m phần tử đầu tiên
    int *result = (int *)malloc(m * sizeof(int));
    if (result == NULL)
    {
        fprintf(stderr, "Error: Memory allocation failed\n");
        free(temp);
        return NULL;
    }
    for (size_t i = 0; i < m; i++)
    {
        result[i] = temp[i];
    }

    // Giải phóng mảng tạm
    free(temp);

    return result;
}
