mergeInto(LibraryManager.library,
{
    radians: function(a) { radian(a); },
    ellipse: function(a,b,c,d) { ellipse(a,b,c,d); },
    rect: function(a,b,c,d) { rect(a,b,c,d); },
    createCanvas: function(x, y) { createCanvas(x, y); },
    background: function(c) { background(c); },
    fill: function(c) { fill(c); },
    stroke: function(c) { stroke(c); },
    point: function(x, y) { point(x, y); },
    line: function(a, b, c, d) { line(a, b, c, d); },
    frameRate: function(a) { frameRate(a); },
    noStroke: function() { noStroke(); },
    noLoop: function() { noLoop(); }
});
