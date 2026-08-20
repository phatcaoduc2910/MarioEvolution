# MarioEvolution

Dự án bài tập lớn môn Lập trình nâng cao (INT2215) do nhóm 9 thực hiện

## Thành viên nhóm

| Thành viên | Phần việc chính |
| --- | --- |
| **Cao Đức Phát (24020264)** | Xây dựng khung `Game`, game loop theo trạng thái và fixed timestep; xử lý input, di chuyển Player, collision, stomp/flag; phát triển camera, viewport, respawn và luồng bắn fireball. |
| **Nguyễn Đức Toàn (24020327)** | Phát triển model và world, level codec/map editor, hệ thống texture/animation/effect; xây dựng enemy, boss gorilla, boss arena và các bài test gameplay/boss. |
| **Nguyễn Thanh Trung (24020336)** | Phát triển màn hình menu/pause/kết quả, âm thanh và điều khiển âm lượng; hoàn thiện HUD, background/parallax, giao diện gameplay và hỗ trợ logic brick/item/flag. |

## Báo cáo và video demo

[Link](https://drive.google.com/drive/folders/1ZvscRZjQHWjBlYDBgAc-RJaW6vBpal37?usp=drive_link)

## Build

Toàn bộ game và test dùng Makefile tại thư mục gốc:

```powershell
make create
make run
make test
make run-map-editor
make clean
```

Project dùng C++17 trên MSYS2 UCRT64 với SDL2, SDL2_image và SDL2_mixer.
`make create` tạo `MarioEvolution.exe`; `make run` build nếu cần rồi chạy game.

## Assets

Asset đang dùng được phân loại trong [`assets/`](assets/README.md).
`TextureManager` nạp trực tiếp texture từ các thư mục `backgrounds/`,
`characters/`, `effects/`, `enemies/`, `items/`, `tiles/` và `ui/`.
Âm thanh được nạp từ `assets/audio/`.

## Map

Game và map editor dùng các file `.map` trong `assets/maps/`. Chọn map ở menu
chính trước khi chơi hoặc mở editor:

```text
B  StandardBrick
?  CoinBrick
M  MushroomBrick
F  FlowerBrick
o  Coin tự do
g  Goomba
k  Koopa xanh
r  Koopa đỏ
p  Piranha Plant
P  Điểm spawn của Mario
G  Boss gorilla
s  Điểm mọc Piranha của boss arena
!  Flag
.  Ô trống
```

## Boss battle

Boss có 6 máu và chỉ bị thương khi trúng một chiếc mai rùa đang trượt. Người chơi cần đạp
Koopa để nó rút vào mai, sau đó đá chiếc mai về phía boss. Nhảy trực tiếp lên
boss hoặc bắn fireball đều không làm boss mất máu; mỗi chiếc mai chỉ đánh
trúng boss được một lần.

- Khi còn 6 đến 4 máu, boss chủ yếu ném Koopa và chưa biết né đòn.
- Khi còn 3 đến 2 máu, boss nổi giận, có thể lao về phía Mario hoặc đập mạnh
  xuống đất. Trước mỗi đòn đều có dấu hiệu để người chơi kịp tránh.
- Khi chỉ còn 1 máu, Piranha Plant bắt đầu mọc lên và Koopa rơi từ trên cao
  xuống. Boss có 55% khả năng né mai rùa, nhưng không né quá hai lần liên tiếp.
- Khi hết máu, boss ngừng tấn công, các kẻ địch còn lại được dọn khỏi màn hình.
  Màn chơi hoàn thành sau khi hoạt ảnh boss bị hạ kết thúc.

## Controls

### Menu và gameplay

- `Up`/`Down`: chọn mục trong menu; `Left`/`Right`: chọn map; `Enter`: xác nhận.
- Có thể click trực tiếp các nút, nút `-`/`+` dùng để đổi map.
- `A`/`Left`: đi trái; `D`/`Right`: đi phải.
- `Space`: nhảy.
- `P` hoặc `Esc`: tạm dừng/tiếp tục.
- Tại màn hình hoàn thành hoặc game over: `Enter` chơi lại, `Esc` về menu.

Mỗi màn bắt đầu với 600 giây và Mario có 3 mạng. Mỗi coin được 200 điểm;
khi hoàn thành màn, mỗi giây còn lại được đổi thành 10 điểm. Điểm được giữ khi
retry, còn timer, coin, kẻ địch và vị trí người chơi được đặt lại theo map.

## Map Editor

Map editor chạy trực tiếp trong cửa sổ game. Chọn `MAP EDITOR` ở menu chính để
mở và nhấn `Esc` để quay lại menu.

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
