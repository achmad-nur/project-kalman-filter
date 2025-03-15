<!doctype html>
<html lang="en">

<head>
    <!-- Required meta tags -->
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">

    <!-- Bootstrap CSS -->
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-EVSTQN3/azprG1Anm3QDgpJLIm9Nao0Yz1ztcQTwFspd3yD65VohhpuuCOmLASjC" crossorigin="anonymous">

    <title>ESP32</title>
    <script type="text/javascript" src="jquery/jquery.min.js"></script>
    <script type="text/javascript" src="jquery/jquery-latest.js"></script>
    <script type="text/javascript" src="jquery/js/jquery-3.4.0.min.js"></script>
    <script type="text/javascript" src="jquery/js/mdb.min.js"></script>

    <!-- load otomatis -->
    <script type="text/javascript">
        $(document).ready(function() {
            setInterval(function() {
                $("#nilaiph").load('nilaiph.php');
                $("#nilai_kalman").load('nilai_kalman.php');
            }, 1000);
        });
    </script>

    <!-- memanggil grafik -->
    <script type="text/javascript">
        var refreshid = setInterval(function() {
            $("#grafik").load('grafik.php');
        }, 1000);
    </script>

</head>

<body>
    <div class="container text-center mt-3">

        <h2>Data Perangkat</h2>

        <div class="d-inline-flex mt-1">
            <!-- menampilkan data sensor pH -->
            <div class="card text-center border-1">
                <div class="card-header fs-3 fw-bold bg-warning border-1">
                    Data pH
                </div>
                <div class="card-body fs-4">
                    <span id="nilaiph"> 0 </span>
                </div>
            </div>

            <!-- menampilkan data sensor pH dengan Kalman Filter -->
            <div class="card text-center border-1">
                <div class="card-header fs-3 fw-bold bg-info">
                    Data Kalman Filter
                </div>
                <div class="card-body fs-4">
                    <span id="nilai_kalman"> 0 </span>
                </div>
            </div>
        </div>
    </div>

    <div class="container text-center mt-2">

        <h2>Grafik Data Perangkat</h2>
        <p>(Data yang ditampilkan adalah 10 data terakhir)</p>

        <div class="container mb-5 text-center" id="grafik" style="width: 50%"></div>
    </div>


    <!-- Optional JavaScript; choose one of the two! -->

    <!-- Option 1: Bootstrap Bundle with Popper -->
    <!-- <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/js/bootstrap.bundle.min.js" integrity="sha384-MrcW6ZMFYlzcLA8Nl+NtUVF0sA7MsXsP1UyJoMp4YLEuNSfAP+JcXn/tWtIaxVXM" crossorigin="anonymous"></script> -->

    <!-- Option 2: Separate Popper and Bootstrap JS -->
    <!--
    <script src="https://cdn.jsdelivr.net/npm/@popperjs/core@2.9.2/dist/umd/popper.min.js" integrity="sha384-IQsoLXl5PILFhosVNubq5LC7Qb9DXgDA9i+tQ8Zj3iwWAwPtgFTxbJ8NT4GN1R8p" crossorigin="anonymous"></script>
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/js/bootstrap.min.js" integrity="sha384-cVKIPhGWiC2Al4u+LWgxfKTRIcfu0JTxR+EQDz/bgldoEyl4H0zUF0QKbrJ0EcQF" crossorigin="anonymous"></script>
    -->
</body>

</html>