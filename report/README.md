# Hướng dẫn biên dịch báo cáo

## Yêu cầu

- TeX Live 2020+ hoặc MiKTeX
- Các package LaTeX cần thiết:
  - vietnam
  - amsmath, amsfonts, amssymb
  - graphicx
  - hyperref
  - listings
  - xcolor
  - geometry
  - fancyhdr
  - titlesec
  - booktabs
  - longtable
  - enumitem

## Cài đặt (Ubuntu/WSL)

```bash
sudo apt-get install texlive-full
```

## Biên dịch

```bash
# Biên dịch PDF
pdflatex baocao.tex
bibtex baocao    # Nếu có bibliography
pdflatex baocao.tex
pdflatex baocao.tex

# Hoặc dùng Makefile
make
```

## Kết quả

File PDF sẽ được tạo: `baocao.pdf`

## Lưu ý

- File `baocao.tex` sử dụng hình ảnh từ thư mục `images/`
- Đảm bảo có file `images/bieudousecase.png` hoặc sửa đường dẫn trong file `.tex`
- Nếu thiếu package, cài thêm bằng:
  - TeX Live: `tlmgr install <package>`
  - MiKTeX: tự động cài khi compile

