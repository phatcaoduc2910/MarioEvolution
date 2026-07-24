# Map Editor

Map editor được nhúng trực tiếp trong cửa sổ game và chỉnh sửa `LevelData` mà
game đang dùng. Thư mục này chỉ giữ tài liệu tương thích cho luồng cũ.

## Build và chạy

```powershell
make map-editor
make run-map-editor
```

Hai lệnh đều build game chính; `run-map-editor` mở game. Bắt đầu màn chơi rồi
nhấn `0` để bật palette.

## Điều khiển

- Phím `0` bật hoặc tắt chế độ editor.
- Phím `1`: `B` - StandardBrick.
- Phím `2`: `?` - CoinBrick.
- Phím `3`: `M` - MushroomBrick.
- Phím `4`: `F` - FlowerBrick.
- Phím `5`: `o` - Coin tự do.
- Phím `E` lấy tile dưới con trỏ làm brush.
- Phím `R` tạo lại level với hàng gạch nền.
- Click palette hoặc dùng phím `1` đến `5` để chọn brush.
- `Ctrl+S` lưu map đang mở; map mới sẽ chuyển sang nhập tên Save As.
- `Tab` hoặc click tab `MAPS` để mở palette quản lý map.
- `W-`, `W+`, `H-`, `H+` đổi kích thước và giữ phần tile còn nằm trong biên.
  Phím tắt tương ứng là `[`/`]` cho chiều rộng, `-`/`=` cho chiều cao.
- `N`/`NEW` tạo map mới; `Ctrl+S`/`SAVE` lưu file hiện tại.
- `F2`, `Ctrl+Shift+S` hoặc `SAVE AS` nhập tên file mới; `Enter` lưu, `Esc` hủy.
- Danh sách `SAVED MAPS` hiển thị mọi file `.map` trong `assets/`; click để mở,
  lăn chuột để cuộn và nhấn `F5` để làm mới.
- Thao tác tạo/mở làm mất thay đổi chưa lưu phải được lặp lại để xác nhận.
- Giữ chuột trái và kéo để vẽ; bắt đầu trên tile đang chọn sẽ xóa cả nét.
- Phím mũi tên hoặc `WASD` di chuyển camera.
- Phím `Esc` đóng editor.

Project Scratch gốc lưu tile theo cột. Phiên bản C++ giữ row-major và dùng `.`
cho ô trống. Game và editor cùng đọc một file map ký tự.

## Nguồn tham khảo

- [Tile Scrolling Platformer | 5. The Level Editor](https://www.youtube.com/watch?v=t9ahg3B3pzk)
- [Project Scratch chính chủ hoàn thành bài 6](https://scratch.mit.edu/projects/522557780/)
