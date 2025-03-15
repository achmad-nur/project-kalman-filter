<?php

$konek = mysqli_connect("localhost", "root", "", "smarthidroponik");

$sql = mysqli_query($konek, "SELECT * FROM tb_data_sensor ORDER BY id DESC");

// Periksa apakah hasil kueri mengembalikan baris
if (mysqli_num_rows($sql) > 0) {
    // Ambil data dari baris pertama
    $data = mysqli_fetch_array($sql);
    $ph = $data['ph'];
} else {
    // Jika tidak ada baris yang dikembalikan, atur $ph ke 0
    $ph = 0;
}

echo $ph;