<?php

if (isset($_GET["long"])) {
    for ($i = 0; $i < 4238; $i++) {
        echo chr(($i % 26) + 97);
    }
} else if (isset($_GET["body"])) {
    echo file_get_contents("php://input");
} else if (isset($_GET["agent"])) {
    echo $_SERVER["HTTP_USER_AGENT"];
} else if (isset($_GET["follow"])) {
    header("Location: http://dordnung.de/sourcemod/system2/testPage.php?followed", true, 301);
    header("System2Follow: true");
} else if (isset($_GET["followed"])) {
    header("System2Followed: true");
    echo $_SERVER["HTTP_REFERER"];
} else if (isset($_GET["timeout"])) {
    sleep(2);
} else if (isset($_GET["auth"])) {
    echo $_SERVER["PHP_AUTH_USER"] . ":" . $_SERVER["PHP_AUTH_PW"];
} else if (isset($_GET["method"])) {
    if ($_SERVER["REQUEST_METHOD"] === "HEAD") {
        header("System2Head: true");
    } else {
        echo $_SERVER["REQUEST_METHOD"];
    }
} else if (isset($_GET["header"])) {
    foreach (getallheaders() as $name => $value) {
        if (strcasecmp($name, "System2Test") == 0) {
            echo $value;
        }
    }
} else if (isset($_GET["deflate"])) {
    header("Content-Encoding: deflate");
    echo gzdeflate("This is deflated content");
}