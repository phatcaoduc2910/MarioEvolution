# MarioEvolution

## Thành viên nhóm

| Thành viên | Phần việc chính |
| --- | --- |
| **Cao Đức Phát** | Xây dựng khung `Game`, game loop theo trạng thái và fixed timestep; xử lý input, di chuyển Player, collision, stomp/flag; phát triển camera, viewport, respawn và luồng bắn fireball. |
| **Nguyễn Đức Toàn** | Phát triển model và world, level codec/map editor, hệ thống texture/animation/effect; xây dựng enemy, boss gorilla, boss arena và các bài test gameplay/boss. |
| **Nguyễn Thanh Trung** | Phát triển màn hình menu/pause/kết quả, âm thanh và điều khiển âm lượng; hoàn thiện HUD, background/parallax, giao diện gameplay và hỗ trợ logic brick/item/flag. |

## Giới thiệu

MarioEvolution là game platformer 2D lấy cảm hứng từ Mario, được xây dựng để
minh họa kiến trúc game theo hướng đối tượng bằng C++ và SDL2. Người chơi điều
khiển Mario vượt chướng ngại vật, thu thập vật phẩm, chiến đấu với kẻ địch và
hoàn thành màn chơi bằng cách chạm cờ hoặc đánh bại boss.

Phạm vi hệ thống gồm gameplay chạy cục bộ trên desktop, quản lý màn chơi,
camera, va chạm, hoạt ảnh, âm thanh, giao diện, map editor và kiểm thử tự động.
Dự án không bao gồm multiplayer, kết nối mạng hoặc lưu tiến trình người chơi.

## Công nghệ và môi trường

- Ngôn ngữ: C++17.
- Thư viện: SDL2, SDL2_image và SDL2_mixer.
- Build: GNU Make, `g++`, `pkg-config`; executable được liên kết tĩnh.
- Môi trường mục tiêu: Windows 10/11 với MSYS2 UCRT64.
- Tài nguyên: PNG cho hình ảnh, WAV/MP3 cho âm thanh và định dạng `.map` riêng
  cho màn chơi.

### Cài đặt

1. Cài [MSYS2](https://www.msys2.org/) vào đường dẫn mặc định
   `C:\msys64`.
2. Mở terminal **MSYS2 UCRT64** và cài các gói cần thiết:

```bash
pacman -Syu
pacman -S --needed make mingw-w64-ucrt-x86_64-gcc \
  mingw-w64-ucrt-x86_64-pkgconf mingw-w64-ucrt-x86_64-SDL2 \
  mingw-w64-ucrt-x86_64-SDL2_image mingw-w64-ucrt-x86_64-SDL2_mixer
```

3. Mở terminal tại thư mục gốc của repository. Không di chuyển thư mục
   `assets/` vì game nạp tài nguyên bằng đường dẫn tương đối.

## Cấu trúc dự án

```text
MarioEvolution/
├── assets/                 Hình ảnh, âm thanh và các file map
├── include/                Header C++
│   ├── controller/         Input, collision và điều phối boss arena
│   ├── core/               Kiểu dữ liệu chung và lớp Game
│   ├── model/              Player, enemy, item, world và level data
│   ├── service/            Audio, level codec và map editor
│   └── view/               Camera, animation, texture và renderer
├── src/                    Phần cài đặt tương ứng với include/
├── tests/                  Unit test và boss acceptance test
├── Makefile                Lệnh build, chạy và kiểm thử
└── README.md               Tài liệu dự án
```

Các module chính:

- `core`: khởi tạo SDL và điều khiển game loop theo `GameState`.
- `model`: lưu trạng thái và luật gameplay, không phụ thuộc vào giao diện.
- `controller`: xử lý input, va chạm và lịch hoạt động của boss arena.
- `view`: tải texture, cập nhật animation và render thế giới lên viewport.
- `service`: âm thanh, đọc/ghi map và các thao tác của map editor.

## Build và chạy

Toàn bộ game và test dùng `Makefile` tại thư mục gốc:

```powershell
make create
make run
make test
make test-boss-acceptance
make run-map-editor
make clean
```

- `make create`: build `MarioEvolution.exe`.
- `make run`: build nếu cần rồi chạy game.
- `make test`: chạy toàn bộ unit test thông thường.
- `make test-boss-acceptance`: chạy bot kiểm tra khả năng hoàn thành boss arena.
- `make run-map-editor`: chạy game; chọn `MAP EDITOR` tại menu chính.
- `make clean`: xóa executable và thư mục build.

## Chức năng đã hoàn thành

- Game loop theo trạng thái: menu, chơi, tạm dừng, map editor, hoàn thành màn
  và game over.
- Di chuyển, nhảy, trọng lực, va chạm theo hai trục, camera hai chiều và
  viewport bám theo người chơi.
- Ba trạng thái Mario: Small, Big và Fire; nhận sát thương, bất tử tạm thời,
  mất mạng, respawn và bắn fireball.
- Goomba, Koopa xanh/đỏ, mai rùa trượt và Piranha Plant với hành vi riêng.
- Gạch thường, gạch vật phẩm, coin, mushroom, fire flower và cột cờ.
- Điểm số, coin, bộ đếm thời gian, số mạng, HUD và màn hình kết quả.
- Hoạt ảnh nhân vật/kẻ địch, hiệu ứng va chạm, âm thanh và nhạc nền.
- Chọn map ở menu; đọc, ghi, tạo mới và thay đổi kích thước map trong editor.
- Boss gorilla ba phase, đòn Charge/Ground Slam, né shell và hazard arena.
- Unit test cho level codec, tile catalog, map editor, camera, collision,
  chuyển động, animation và boss; có thêm boss acceptance test.

## Báo cáo và video demo

- Link: https://drive.google.com/drive/folders/1ZvscRZjQHWjBlYDBgAc-RJaW6vBpal37?usp=sharing

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

`assets/maps/boss_arena.map` là arena của boss gorilla. Boss có 6 HP và chỉ
nhận damage từ mai rùa đang trượt (`ShellSliding`): boss ném Koopa, Mario đạp
Koopa thành mai rồi đá ngược lại. Stomp thẳng vào boss và fireball không trừ
máu, mỗi mai rùa chỉ gây tối đa một damage.

- Phase 1 (HP 6-4): boss chỉ ném Koopa, không né.
- Phase 2 (HP 3-2): Enraged, thêm Charge và Ground Slam, mỗi đòn có telegraph
  và recovery.
- Phase 3 (HP 1): arena bật hazard - Piranha mọc tại các điểm `s` và Koopa rơi
  từ trên xuống; boss né shell với xác suất 55%, tối đa hai lần liên tiếp.
- HP 0: hazard dừng, arena dọn Koopa/Piranha rồi chuyển Level Complete sau khi
  death animation chạy xong.

Thanh HP boss nằm giữa mép trên màn hình và đổi màu khi vào Enraged/Last HP.

`make test-boss` chạy unit test của boss/arena. `make test-boss-acceptance`
chạy một bot chỉ dùng input người chơi để hạ boss trên arena; chạy lại nó sau
khi chỉnh các thông số tuning trong `BossArenaController` hoặc `Boss`.

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
