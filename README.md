# MarioEvolution

## Build

Toàn bộ game và tool dùng Makefile tại thư mục gốc:

```powershell
make create
make run
make test
make run-map-editor
make clean
```

Project dĂ¹ng C++17 trĂªn MSYS2 UCRT64 vá»›i SDL2, SDL2_image vĂ  SDL2_mixer.
`make create` táº¡o `MarioEvolution.exe`; `make run` build náº¿u cáº§n rá»“i cháº¡y game.

## Assets

Asset đang dùng được phân loại trong [`assets/`](assets/README.md). Hai atlas ở
`assets/runtime/` được tạo từ sprite mới cho renderer hiện tại. Bộ asset cũ được
giữ nguyên tại `backups/assets_before_tile_scrolling_import_2026-08-12/`.

## Map

Game và map editor dùng chung `assets/maps/level1.map`:

```text
B  StandardBrick
?  CoinBrick
M  MushroomBrick
F  FlowerBrick
g  Goomba
!  Flag
o  Coin tự do
.  Ô trống
```

## Controls

### Menu vĂ  gameplay

- `Up`/`Down`: chá»n má»¥c trong menu; `Enter`: xĂ¡c nháº­n.
- `A`/`Left`: Ä‘i trĂ¡i; `D`/`Right`: Ä‘i pháº£i.
- `Space`: nháº£y.
- `P` hoáº·c `Esc`: táº¡m dá»«ng/tiáº¿p tá»¥c.
- `0`: má»Ÿ map editor khi Ä‘ang chÆ¡i.
- Táº¡i mĂ n hĂ¬nh hoĂ n thĂ nh hoáº·c game over: `Enter` chÆ¡i láº¡i, `Esc` vá» menu.

## Map Editor

Map editor chạy trực tiếp trong cửa sổ game. Bắt đầu game rồi nhấn `0` để mở
hoặc đóng editor.

- Click tile trong palette hoặc dùng phím `1` đến `5` để chọn brush.
- Giữ chuột trái và kéo trên map để đặt tile; bắt đầu trên tile cùng loại để xóa.
- Phím `E` lấy tile dưới con trỏ làm brush.
- Phím mũi tên hoặc `WASD` di chuyển camera.
- `Tab` hoặc click `TILES`/`MAPS` để đổi trang palette.
- Trong trang `MAPS`, click `W-`, `W+`, `H-`, `H+` để đổi kích thước map.
  Có thể dùng `[`/`]` cho chiều rộng và `-`/`=` cho chiều cao.
- `N` hoặc nút `NEW` tạo map mới với kích thước hiện tại.
- `Ctrl+S` hoặc nút `SAVE` lưu map hiện tại.
- `F2`, `Ctrl+Shift+S` hoặc nút `SAVE AS` để nhập tên map mới; `Enter` lưu,
  `Esc` hủy. Tên nhận chữ, số, `_`, `-` và được lưu trong `assets/maps/`.
- Click tên trong `SAVED MAPS` để mở map; lăn chuột để xem danh sách dài và
  nhấn `F5` để quét lại thư mục.
- Khi có thay đổi chưa lưu, lặp lại thao tác `NEW`/mở map để xác nhận bỏ thay đổi.
- Phím `R` tạo lại map và `Esc` đóng editor.
