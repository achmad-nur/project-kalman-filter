<?php

$konek = mysqli_connect("localhost", "root", "", "smarthidroponik");

$ph = $_GET['ph'];
$kalman = $_GET['kalman'];
$time = date('Y-m-d H:i:s');

// Reset AUTO_INCREMENT
mysqli_query($konek, "ALTER TABLE tb_data_sensor AUTO_INCREMENT=1");

// Simpan ke tabel 
$simpan = mysqli_query($konek, "INSERT INTO tb_data_sensor(ph, kalman, time) VALUES('$ph', '$kalman', '$time')");
if ($simpan) {
    echo "Sukses dikirim";
} else {
    echo "Gagal dikirim";
}