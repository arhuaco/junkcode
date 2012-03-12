DVD:

To check md5sums:

  md5sums -c MD5SUMS.txt

To generate md5sums:

  find . -type f -exec md5sum {} \; > MD5SUMS.txt
