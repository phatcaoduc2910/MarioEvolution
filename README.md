# MarioEvolution

## Build

Toàn bộ game và tool dùng Makefile tại thư mục gốc:

```powershell
make create
make run
make run-map-editor
make render-preview
make run-render-preview
make test
make clean
```

## Map

Game và map editor dùng chung `assets/level1.map`:

```text
B  StandardBrick
?  CoinBrick
M  MushroomBrick
F  FlowerBrick
o  Coin tự do
.  Ô trống
```

## Map Editor

Map editor chạy trực tiếp trong cửa sổ game. Bắt đầu game rồi nhấn `0` để mở
hoặc đóng editor.

- Click tile trong palette hoặc dùng phím `1` đến `5` để chọn brush.
- Giữ chuột trái và kéo trên map để đặt tile; bắt đầu trên tile cùng loại để xóa.
- Phím `E` lấy tile dưới con trỏ làm brush.
- Phím mũi tên hoặc `WASD` di chuyển camera.
- `Ctrl+S` lưu vào `assets/level1.map`.
- Phím `R` tạo lại map và `Esc` đóng editor.
