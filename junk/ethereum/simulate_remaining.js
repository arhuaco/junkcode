function Simulate(){
    var latestBlock = eth.getBlock("latest");
    var latest = latestBlock.number;

    var n_inc = 0;
    var n_dec = 0;

    console.log('Looking back 10 minutes (Approx 40 blocks)');
    lookback = 40;
    var prev = eth.getBlock(latest - lookback).gasLimit
    var current;
    for (i = latest - lookback + 1; i <= latest; i++){
        current = eth.getBlock(i).gasLimit;
        if (current > prev) {
            ++n_inc;
        } else {
            ++n_dec;
        }
        prev = current;
     }

    console.log('Voting up: ' + n_inc);
    console.log('Votoing down: ' + n_dec);
    console.log('Simulating... Using a line would be enough.');

    var blocks_remaining = 0;
    while (current < 21000) {
        var delta = current / 1024;
        if (Math.random() < n_dec / n_inc) {
            current -= delta;
        } else {
            current += delta;
        }
        current = Math.max(5000, current);
        blocks_remaining += 1;
    }
    console.log('Blocks remaining: ' + blocks_remaining);
    console.log('Hours remaining: ' + (blocks_remaining / 4 / 60));
}
Simulate()
