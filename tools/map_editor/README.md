# Map Editor

Map editor SDL2 chuyển hành vi từ bài 5 của series Tile Scrolling Platformer
sang C++ và chỉnh sửa trực tiếp một `LevelData` trong bộ nhớ.

## Build và chạy

```powershell
make map-editor
make run-map-editor
```

## Điều khiển

- Phím `0` bật hoặc tắt chế độ editor.
- Phím `1`: `B` - StandardBrick.
- Phím `2`: `?` - CoinBrick.
- Phím `3`: `M` - MushroomBrick.
- Phím `4`: `F` - FlowerBrick.
- Phím `5`: `o` - Coin tự do.
- Phím `E` lấy tile dưới con trỏ làm brush.
- Phím `R` tạo lại level với hàng gạch nền.
- Phím `S` lưu vào `assets/level1.map`.
- Giữ chuột trái và kéo để vẽ; bắt đầu trên tile đang chọn sẽ xóa cả nét.
- Phím mũi tên hoặc `WASD` di chuyển camera.
- Phím `Esc` đóng editor.

Project Scratch gốc lưu tile theo cột. Phiên bản C++ giữ row-major và dùng `.`
cho ô trống. Game và editor cùng đọc một file map ký tự.

## Nguồn tham khảo

- [Tile Scrolling Platformer | 5. The Level Editor](https://www.youtube.com/watch?v=t9ahg3B3pzk)
- [Project Scratch chính chủ hoàn thành bài 6](https://scratch.mit.edu/projects/522557780/)
