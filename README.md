# SoalShiftSISOP20_modul4_B06
Oleh Kelompok B06
* _Clement Prolifel Priyatama (0511184000013)_
* _Aaron Astonvilla Rompis (05111840000131)_

----------------------------------------------------------------
## Soal
* [Soal](#Soal)
  * [Detail 1](#Detail-1)
  * [Detail 2](#Detail-2)
  * [Detail 3](#Detail-3)
  * [Detail 4](#Detail-4)

----------------------------------------------------------------

### Soal
   _**Soal:**_\
   Di suatu perusahaan, terdapat pekerja baru yang super jenius, ia bernama Jasir. Jasir baru bekerja selama seminggu di perusahan itu, dalam waktu seminggu tersebut ia selalu terhantui oleh ketidak amanan dan ketidak efisienan file system yang digunakan perusahaan tersebut. Sehingga ia merancang sebuah file system yang sangat aman dan efisien. Pada segi keamanan, Jasir telah menemukan 2 buah metode enkripsi file. Pada mode enkripsi pertama, nama file-file pada direktori terenkripsi akan dienkripsi menggunakan metode substitusi. Sedangkan pada metode enkripsi yang kedua, file-file pada direktori terenkripsi akan di-split menjadi file-file kecil. Sehingga orang-orang yang tidak menggunakan filesystem rancangannya akan kebingungan saat mengakses direktori terenkripsi tersebut. Untuk segi efisiensi, dikarenakan pada perusahaan tersebut sering dilaksanakan sinkronisasi antara 2 direktori, maka Jasir telah merumuskan sebuah metode agar filesystem-nya mampu mengsingkronkan kedua direktori tersebut secara otomatis. Agar integritas filesystem tersebut lebih terjamin, maka setiap command yang dilakukan akan dicatat kedalam sebuah file log.
(catatan: filesystem berfungsi normal layaknya linux pada umumnya)
(catatan: mount source (root) filesystem adalah direktori /home/[user]/Documents)

#### Detail 1
_**Penjelasan:**_\
Enkripsi versi 1:
Jika sebuah direktori dibuat dengan awalan “encv1_”, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode enkripsi v1.
Jika sebuah direktori di-rename dengan awalan “encv1_”, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode enkripsi v1.
Apabila sebuah direktori terenkripsi di-rename menjadi tidak terenkripsi, maka isi adirektori tersebut akan terdekrip.
Setiap pembuatan direktori terenkripsi baru (mkdir ataupun rename) akan tercatat ke sebuah database/log berupa file.
Semua file yang berada dalam direktori ter enkripsi menggunakan caesar cipher dengan key.

```
9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO
```

Misalkan ada file bernama “kelincilucu.jpg” dalam directory FOTO_PENTING, dan key yang dipakai adalah 10
“encv1_rahasia/FOTO_PENTING/kelincilucu.jpg” => “encv1_rahasia/ULlL@u]AlZA(/g7D.|_.Da_a.jpg
Note : Dalam penamaan file ‘/’ diabaikan, dan ekstensi tidak perlu di encrypt.
Metode enkripsi pada suatu direktori juga berlaku kedalam direktori lainnya yang ada didalamnya.

_**Penyelesaian**_
Kami membuat fungsi encrypt dan decrypt encv1_ di bagian lain, tetapi saat dicoba tidak bisa.

**Permasalahan**
Kami tidak mampu menyelesaikan lagi mas mbak.
