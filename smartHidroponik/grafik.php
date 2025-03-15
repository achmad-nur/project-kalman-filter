<?php

$konek = mysqli_connect("localhost", "root", "", "smarthidroponik");

$sql_id = mysqli_query($konek, "SELECT MAX(id) FROM tb_data_sensor");
$data_id = mysqli_fetch_array($sql_id);
$id_akhir = $data_id['MAX(id)'];
$id_awal = $id_akhir - 19;

$time = mysqli_query($konek, "SELECT time FROM tb_data_sensor WHERE id>='$id_awal' AND id<='$id_akhir' ORDER BY id ASC");
$ph= mysqli_query($konek, "SELECT ph FROM tb_data_sensor WHERE id>='$id_awal' AND id<='$id_akhir' ORDER BY id ASC");
$kalman = mysqli_query($konek, "SELECT kalman FROM tb_data_sensor WHERE id>='$id_awal' AND id<='$id_akhir' ORDER BY id ASC");

?>

<!-- tampilan grafik -->
<div class="card">
    <div class="card-header text-center bg-primary text-light fs-5">
        Grafik
    </div>
    <div class="card-body">
        <canvas id="myChart"></canvas>
        <script type="text/javascript">
            var canvas = document.getElementById('myChart');

            var data = {
                labels: [
                    <?php
                    while ($data_time = mysqli_fetch_array($time)) {
                        echo '"' . $data_time['time'] . '",';
                    }
                    ?>
                ],
                datasets: [{
                        label: "pH",
                        fill: true,
                        backgroundColor: 'transparent',
                        borderColor: 'blue',
                        lineTension: 0.5,
                        pointRadius: 2,
                        data: [
                            <?php
                            while ($data_ph = mysqli_fetch_array($ph)) {
                                echo $data_ph['ph'] . ',';
                            }
                            ?>
                        ]
                    },
                    {
                        label: "Kalman Filter",
                        fill: true,
                        backgroundColor: 'transparent',
                        borderColor: 'green',
                        lineTension: 0.5,
                        pointRadius: 2,
                        data: [
                            <?php
                            while ($data_kalman = mysqli_fetch_array($kalman)) {
                                echo $data_kalman['kalman'] . ',';
                            }
                            ?>
                        ]
                    }
                ]
            };

            // option grafik
            var option = {
                showLines: true,
                animation: {
                    duration: 0
                }
            };

            // cetak grafik
            var myLineChart = Chart.Line(canvas, {
                data: data,
                options: option,
            });
        </script>
    </div>
</div>