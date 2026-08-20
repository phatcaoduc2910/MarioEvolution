# MarioEvolution

MarioEvolution là game platform 2D lấy cảm hứng từ Super Mario, được nhóm 9
phát triển cho bài tập lớn môn Lập trình nâng cao (INT2215). Người chơi điều
khiển Mario vượt chướng ngại vật, thu thập vật phẩm, chiến đấu với kẻ địch và
hoàn thành các màn chơi, bao gồm một boss arena nhiều giai đoạn.

Phạm vi dự án gồm game chơi đơn trên máy tính, hệ thống menu và trạng thái
game, vật lý và va chạm, camera, đồ họa và âm thanh, bản đồ lưu cục bộ cùng
map editor tích hợp. Dự án không bao gồm chế độ nhiều người chơi hoặc dịch vụ
trực tuyến.

## Thành viên nhóm

| Thành viên | Phần việc chính |
| --- | --- |
| **Cao Đức Phát (24020264)** | Xây dựng khung `Game`, game loop theo trạng thái và fixed timestep; xử lý input, di chuyển Player, collision, stomp/flag; phát triển camera, viewport, respawn và luồng bắn fireball. |
| **Nguyễn Đức Toàn (24020327)** | Phát triển model và world, level codec/map editor, hệ thống texture/animation/effect; xây dựng enemy, boss gorilla, boss arena và các bài test gameplay/boss. |
| **Nguyễn Thanh Trung (24020336)** | Phát triển màn hình menu/pause/kết quả, âm thanh và điều khiển âm lượng; hoàn thiện HUD, background/parallax, giao diện gameplay và hỗ trợ logic brick/item/flag. |

Nhóm phối hợp tích hợp các module, hoàn thiện tài nguyên, kiểm thử và cân chỉnh
gameplay trên các màn chơi thường lẫn boss arena.

## Chức năng đã hoàn thành

- Menu chính, chọn map, tạm dừng, chơi lại, game over và màn hình hoàn thành.
- Điều khiển Mario, nhảy, va chạm, đạp kẻ địch, bắn fireball, thu thập coin và
  vật phẩm, phá hoặc kích hoạt các loại brick.
- Goomba, Koopa, Piranha Plant, boss gorilla nhiều giai đoạn và cơ chế chiến
  đấu bằng mai rùa.
- Camera theo người chơi, HUD, điểm số, timer, mạng, respawn và lưu điểm khi
  chơi lại màn.
- Texture, sprite animation, hiệu ứng, background parallax, âm thanh và điều
  chỉnh âm lượng.
- Đọc và chọn map từ file, cùng map editor hỗ trợ tạo, sửa, đổi kích thước,
  lưu và mở lại bản đồ.
- Các bài test cho level codec, tile catalog, map editor, camera, collision,
  chuyển động người chơi, animation và boss battle.

## Báo cáo và video demo

[Xem báo cáo PDF và video demo trên Google Drive](https://drive.google.com/drive/folders/1ZvscRZjQHWjBlYDBgAc-RJaW6vBpal37?usp=drive_link)

## Công nghệ và yêu cầu cài đặt

- Ngôn ngữ: C++17.
- Môi trường mục tiêu: Windows với MSYS2 UCRT64.
- Công cụ build: GNU Make, GCC/G++ và pkg-config.
- Thư viện: SDL2, SDL2_image và SDL2_mixer.

Cài [MSYS2](https://www.msys2.org/), mở terminal **MSYS2 UCRT64** rồi chạy:

```bash
pacman -Syu
pacman -S --needed make mingw-w64-ucrt-x86_64-gcc \
  mingw-w64-ucrt-x86_64-pkgconf mingw-w64-ucrt-x86_64-SDL2 \
  mingw-w64-ucrt-x86_64-SDL2_image mingw-w64-ucrt-x86_64-SDL2_mixer
```

## Cấu trúc dự án

```text
MarioEvolution/
|-- src/       Mã nguồn triển khai
|-- include/   Header theo từng module
|-- assets/    Texture, âm thanh và bản đồ
|-- tests/     Các bài test tự động
|-- builds/    Object file và chương trình test sau khi build
|-- Makefile   Cấu hình build, chạy và test
`-- README.md  Tài liệu dự án
```

Các module trong `src/` và `include/`:

- `core`: game loop, trạng thái game và các kiểu dữ liệu dùng chung.
- `controller`: xử lý input, va chạm và điều khiển boss arena.
- `model`: player, enemy, boss, vật phẩm, đối tượng game và world.
- `service`: âm thanh, đọc/ghi level và nghiệp vụ map editor.
- `view`: camera, renderer, UI, HUD, màn hình, texture và animation.

## Build và chạy

Từ thư mục gốc của dự án trong terminal MSYS2 UCRT64, dùng các lệnh:

```bash
make create
make run
make test
make run-map-editor
make clean
```

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
