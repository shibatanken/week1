// random_array_utils.h
#ifndef RANDOM_ARRAY_UTILS_H
#define RANDOM_ARRAY_UTILS_H
#include <stddef.h> // Để sử dụng size_t

// Hoán đổi hai phần tử
void swap(int *a, int *b);

// Lấy ngẫu nhiên m phần tử không trùng từ một mảng n phần tử
// arr: mảng gốc
// size: kích thước mảng gốc
// m: số phần tử cần lấy
// Trả về: con trỏ đến mảng chứa m phần tử đã được chọn ngẫu nhiên
// Ghi chú: Kết quả cần được giải phóng bộ nhớ sau khi sử dụng
int *getRandomSubset(const int *arr, size_t size, size_t m);

#endif // RANDOM_ARRAY_UTILS_H
