function Simulate(){
    var latestBlock = eth.getBlock("latest");
    var latest = latestBlock.number;

    var n_inc = 0;
    var n_dec = 0;

    console.log('Looking back 30 minutes (Approx 120 blocks)');
    lookback = 120;
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

    console.log('Current gas limit (need 105000 for 5 txs/block): ' + current.toFixed(4));
    console.log('Voting up: ' + (100 * n_inc / lookback).toFixed(3) + '%');
    down_percent = 100 * n_dec / lookback;
    console.log('Voting down: ' + down_percent.toFixed(3) + '%');

    if (down_percent > 33.0) {
        console.log('With > 33% voting down it takes a lot of time (more than 150 hours). Turn on miners on with 1.0.1!');
        console.log('https://github.com/arhuaco/junkcode/blob/master/junk/ethereum/simulate_remaining.js');
        return;
    }

    console.log('Simulating...');

    function simulate_once(start) {
        var blocks_remaining = 0;
        var current = start;
        while (current < 21000 * 5) {
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
    console.log('Hours remaining: ' + (blocks_remaining / 4 / 60).toFixed(3));
}
Simulate()
