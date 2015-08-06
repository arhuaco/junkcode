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

    console.log('Voting up: ' + (100 * n_inc / lookback) + '%');
    console.log('Voting down: ' + (100 * n_dec / lookback) + '%');
    console.log('Simulating...');

    function simulate_once(start) {
        var blocks_remaining = 0;
        var current = start;
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
       return blocks_remaining;
    }
    var n_times = 10;
    var total = 0;
    for (i = 0; i < n_times; ++i) {
        total += simulate_once(current);
    }
    var blocks_remaining = total / n_times;
    console.log('Blocks remaining(estimated): ' + Math.floor(blocks_remaining));
    console.log('Hours remaining: ' + (blocks_remaining / 4 / 60));
}
Simulate()
