# Dynamic Round System

🎯 Dynamic Round System (SA-MP)

Sistem Dynamic Round ini memungkinkan server San Andreas Multiplayer untuk membuat, mengedit, dan menghapus area berbentuk lingkaran (round) secara dinamis dengan dukungan database MySQL.

Cocok digunakan untuk:

Area event
Zone PvP
Checkpoint custom
Safe zone / danger zone
⚙️ Features
✅ Create round langsung in-game
✅ Edit posisi, warna, ukuran, world, interior
✅ Auto save ke database (MySQL)
✅ Load otomatis saat server start
✅ Dynamic object & area (streamer)
✅ Support hingga 300 round
📦 Requirements

Pastikan server kamu sudah include:

a_samp
streamer
a_mysql
YSI\y_hooks
YSI\y_iterate
sscanf2
🗄️ Database Structure

Buat tabel rounds:

CREATE TABLE `rounds` (
  `id` INT AUTO_INCREMENT PRIMARY KEY,
  `x` FLOAT,
  `y` FLOAT,
  `z` FLOAT,
  `world` INT,
  `interior` INT,
  `color` INT,
  `size` FLOAT
);
🚀 Commands
🔹 /createround

Membuat round baru di posisi player

/createround [color] [size]

Contoh:

/createround merah 5.0
🔹 /editround

Edit data round

/editround [id] [type] [value]
Type yang tersedia:
pos → update posisi ke posisi player
vw → virtual world
int → interior
color → warna
size → ukuran
delete → hapus round

Contoh:

/editround 1 size 10.0
/editround 1 color biru
/editround 1 delete
🔹 /nearround

Menampilkan ID round terdekat

/nearround
🎨 Color List

Gunakan nama warna berikut:

merah
hijau
biru
kuning
pink
putih
hitam
🔄 System Flow
Server start → Load_Round() ambil data dari MySQL
Round dibuat → masuk ke iterator (YSI)
Object + area dibuat via streamer
Player spawn → semua round ditampilkan
Server shutdown → semua round auto save
🧠 Core Functions
Round_Reset() → reset semua data
Round_Refresh(id) → recreate object & area
Round_Save(id) → simpan ke database
Load_Round() → load dari MySQL
⚠️ Notes
Max round: 300 (bisa diubah di #define MAX_DYNAMIC_ROUND)
Hanya admin level 7+ yang bisa create/edit
Gunakan plugin streamer terbaru agar optimal
Pastikan koneksi MySQL (g_SQL) sudah benar
📌 Tips
Gunakan size kecil (2.0 - 10.0) untuk performa stabil
Jangan terlalu banyak round di satu area
Gunakan warna transparan agar tidak terlalu mencolok
