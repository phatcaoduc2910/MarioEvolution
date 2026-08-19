# MarioEvolution

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
