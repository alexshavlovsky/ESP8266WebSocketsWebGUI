// MIT License:
//
// Copyright (c) 2010-2013, Joe Walnes
//               2013-2017, Drew Noakes
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

/**
 * Smoothie Charts - http://smoothiecharts.org/
 * (c) 2010-2013, Joe Walnes
 *     2013-2017, Drew Noakes
 *
 * v1.0: Main charting library, by Joe Walnes
 * v1.1: Auto scaling of axis, by Neil Dunn
 * v1.2: fps (frames per second) option, by Mathias Petterson
 * v1.3: Fix for divide by zero, by Paul Nikitochkin
 * v1.4: Set minimum, top-scale padding, remove timeseries, add optional timer to reset bounds, by Kelley Reynolds
 * v1.5: Set default frames per second to 50... smoother.
 *       .start(), .stop() methods for conserving CPU, by Dmitry Vyal
 *       options.interpolation = 'bezier' or 'line', by Dmitry Vyal
 *       options.maxValue to fix scale, by Dmitry Vyal
 * v1.6: minValue/maxValue will always get converted to floats, by Przemek Matylla
 * v1.7: options.grid.fillStyle may be a transparent color, by Dmitry A. Shashkin
 *       Smooth rescaling, by Kostas Michalopoulos
 * v1.8: Set max length to customize number of live points in the dataset with options.maxDataSetLength, by Krishna Narni
 * v1.9: Display timestamps along the bottom, by Nick and Stev-io
 *       (https://groups.google.com/forum/?fromgroups#!topic/smoothie-charts/-Ywse8FCpKI%5B1-25%5D)
 *       Refactored by Krishna Narni, to support timestamp formatting function
 * v1.10: Switch to requestAnimationFrame, removed the now obsoleted options.fps, by Gergely Imreh
 * v1.11: options.grid.sharpLines option added, by @drewnoakes
 *        Addressed warning seen in Firefox when seriesOption.fillStyle undefined, by @drewnoakes
 * v1.12: Support for horizontalLines added, by @drewnoakes
 *        Support for yRangeFunction callback added, by @drewnoakes
 * v1.13: Fixed typo (#32), by @alnikitich
 * v1.14: Timer cleared when last TimeSeries removed (#23), by @davidgaleano
 *        Fixed diagonal line on chart at start/end of data stream, by @drewnoakes
 * v1.15: Support for npm package (#18), by @dominictarr
 *        Fixed broken removeTimeSeries function (#24) by @davidgaleano
 *        Minor performance and tidying, by @drewnoakes
 * v1.16: Bug fix introduced in v1.14 relating to timer creation/clearance (#23), by @drewnoakes
 *        TimeSeries.append now deals with out-of-order timestamps, and can merge duplicates, by @zacwitte (#12)
 *        Documentation and some local variable renaming for clarity, by @drewnoakes
 * v1.17: Allow control over font size (#10), by @drewnoakes
 *        Timestamp text won't overlap, by @drewnoakes
 * v1.18: Allow control of max/min label precision, by @drewnoakes
 *        Added 'borderVisible' chart option, by @drewnoakes
 *        Allow drawing series with fill but no stroke (line), by @drewnoakes
 * v1.19: Avoid unnecessary repaints, and fixed flicker in old browsers having multiple charts in document (#40), by @asbai
 * v1.20: Add SmoothieChart.getTimeSeriesOptions and SmoothieChart.bringToFront functions, by @drewnoakes
 * v1.21: Add 'step' interpolation mode, by @drewnoakes
 * v1.22: Add support for different pixel ratios. Also add optional y limit formatters, by @copacetic
 * v1.23: Fix bug introduced in v1.22 (#44), by @drewnoakes
 * v1.24: Fix bug introduced in v1.23, re-adding parseFloat to y-axis formatter defaults, by @siggy_sf
 * v1.25: Fix bug seen when adding a data point to TimeSeries which is older than the current data, by @Nking92
 *        Draw time labels on top of series, by @comolosabia
 *        Add TimeSeries.clear function, by @drewnoakes
 * v1.26: Add support for resizing on high device pixel ratio screens, by @copacetic
 * v1.27: Fix bug introduced in v1.26 for non whole number devicePixelRatio values, by @zmbush
 * v1.28: Add 'minValueScale' option, by @megawac
 *        Fix 'labelPos' for different size of 'minValueString' 'maxValueString', by @henryn
 * v1.29: Support responsive sizing, by @drewnoakes
 * v1.29.1: Include types in package, and make property optional, by @TrentHouliston
 * v1.30: Fix inverted logic in devicePixelRatio support, by @scanlime
 * v1.31: Support tooltips, by @Sly1024 and @drewnoakes
 * v1.32: Support frame rate limit, by @dpuyosa
 */

const char SMOOTHIE_JS[] PROGMEM = ";(function(s){var u={extend:function(){arguments[0]=arguments[0]||{};for(var i=1;i<arguments.length;i++){for(var a in arguments[i]){if(arguments[i].hasOwnProperty(a)){if(typeof(arguments[i][a])==='object'){if(arguments[i][a]instanceof Array){arguments[0][a]=arguments[i][a]}else{arguments[0][a]=u.extend(arguments[0][a],arguments[i][a])}}else{arguments[0][a]=arguments[i][a]}}}}return arguments[0]}};function TimeSeries(a){this.options=u.extend({},TimeSeries.defaultOptions,a);this.clear()}TimeSeries.defaultOptions={resetBoundsInterval:3000,resetBounds:true};TimeSeries.prototype.clear=function(){this.data=[];this.maxValue=Number.NaN;this.minValue=Number.NaN};TimeSeries.prototype.resetBounds=function(){if(this.data.length){this.maxValue=this.data[0][1];this.minValue=this.data[0][1];for(var i=1;i<this.data.length;i++){var a=this.data[i][1];if(a>this.maxValue){this.maxValue=a}if(a<this.minValue){this.minValue=a}}}else{this.maxValue=Number.NaN;this.minValue=Number.NaN}};TimeSeries.prototype.append=function(a,b,c){var i=this.data.length-1;while(i>=0&&this.data[i][0]>a){i--}if(i===-1){this.data.splice(0,0,[a,b])}else if(this.data.length>0&&this.data[i][0]===a){if(c){this.data[i][1]+=b;b=this.data[i][1]}else{this.data[i][1]=b}}else if(i<this.data.length-1){this.data.splice(i+1,0,[a,b])}else{this.data.push([a,b])}this.maxValue=isNaN(this.maxValue)?b:Math.max(this.maxValue,b);this.minValue=isNaN(this.minValue)?b:Math.min(this.minValue,b)};TimeSeries.prototype.dropOldData=function(a,b){var c=0;while(this.data.length-c>=b&&this.data[c+1][0]<a){c++}if(c!==0){this.data.splice(0,c)}};function SmoothieChart(a){this.options=u.extend({},SmoothieChart.defaultChartOptions,a);this.seriesSet=[];this.currentValueRange=1;this.currentVisMinValue=0;this.lastRenderTimeMillis=0}SmoothieChart.defaultChartOptions={millisPerPixel:20,enableDpiScaling:true,yMinFormatter:function(a,b){return parseFloat(a).toFixed(b)},yMaxFormatter:function(a,b){return parseFloat(a).toFixed(b)},maxValueScale:1,minValueScale:1,interpolation:'bezier',scaleSmoothing:0.125,maxDataSetLength:2,scrollBackwards:false,grid:{fillStyle:'#000000',strokeStyle:'#777777',lineWidth:1,sharpLines:false,millisPerLine:1000,verticalSections:2,borderVisible:true},labels:{fillStyle:'#ffffff',disabled:false,fontSize:10,fontFamily:'monospace',precision:2},horizontalLines:[]};SmoothieChart.AnimateCompatibility=(function(){var e=function(b,c){var d=window.requestAnimationFrame||window.webkitRequestAnimationFrame||window.mozRequestAnimationFrame||window.oRequestAnimationFrame||window.msRequestAnimationFrame||function(a){return window.setTimeout(function(){a(new Date().getTime())},16)};return d.call(window,b,c)},cancelAnimationFrame=function(b){var c=window.cancelAnimationFrame||function(a){clearTimeout(a)};return c.call(window,b)};return{requestAnimationFrame:e,cancelAnimationFrame:cancelAnimationFrame}})();SmoothieChart.defaultSeriesPresentationOptions={lineWidth:1,strokeStyle:'#ffffff'};SmoothieChart.prototype.addTimeSeries=function(a,b){this.seriesSet.push({timeSeries:a,options:u.extend({},SmoothieChart.defaultSeriesPresentationOptions,b)});if(a.options.resetBounds&&a.options.resetBoundsInterval>0){a.resetBoundsTimerId=setInterval(function(){a.resetBounds()},a.options.resetBoundsInterval)}};SmoothieChart.prototype.removeTimeSeries=function(a){var b=this.seriesSet.length;for(var i=0;i<b;i++){if(this.seriesSet[i].timeSeries===a){this.seriesSet.splice(i,1);break}}if(a.resetBoundsTimerId){clearInterval(a.resetBoundsTimerId)}};SmoothieChart.prototype.getTimeSeriesOptions=function(a){var b=this.seriesSet.length;for(var i=0;i<b;i++){if(this.seriesSet[i].timeSeries===a){return this.seriesSet[i].options}}};SmoothieChart.prototype.bringToFront=function(a){var b=this.seriesSet.length;for(var i=0;i<b;i++){if(this.seriesSet[i].timeSeries===a){var c=this.seriesSet.splice(i,1);this.seriesSet.push(c[0]);break}}};SmoothieChart.prototype.streamTo=function(a,b){this.canvas=a;this.delay=b;this.start()};SmoothieChart.prototype.resize=function(){if(!this.options.enableDpiScaling||!window||window.devicePixelRatio===1)return;var a=window.devicePixelRatio;var b=parseInt(this.canvas.getAttribute('width'));var c=parseInt(this.canvas.getAttribute('height'));if(!this.originalWidth||(Math.floor(this.originalWidth*a)!==b)){this.originalWidth=b;this.canvas.setAttribute('width',(Math.floor(b*a)).toString());this.canvas.style.width=b+'px';this.canvas.getContext('2d').scale(a,a)}if(!this.originalHeight||(Math.floor(this.originalHeight*a)!==c)){this.originalHeight=c;this.canvas.setAttribute('height',(Math.floor(c*a)).toString());this.canvas.style.height=c+'px';this.canvas.getContext('2d').scale(a,a)}};SmoothieChart.prototype.start=function(){if(this.frame){return}var a=function(){this.frame=SmoothieChart.AnimateCompatibility.requestAnimationFrame(function(){this.render();a()}.bind(this))}.bind(this);a()};SmoothieChart.prototype.stop=function(){if(this.frame){SmoothieChart.AnimateCompatibility.cancelAnimationFrame(this.frame);delete this.frame}};SmoothieChart.prototype.updateValueRange=function(){var a=this.options,chartMaxValue=Number.NaN,chartMinValue=Number.NaN;for(var d=0;d<this.seriesSet.length;d++){var b=this.seriesSet[d].timeSeries;if(!isNaN(b.maxValue)){chartMaxValue=!isNaN(chartMaxValue)?Math.max(chartMaxValue,b.maxValue):b.maxValue}if(!isNaN(b.minValue)){chartMinValue=!isNaN(chartMinValue)?Math.min(chartMinValue,b.minValue):b.minValue}}if(a.maxValue!=null){chartMaxValue=a.maxValue}else{chartMaxValue*=a.maxValueScale}if(a.minValue!=null){chartMinValue=a.minValue}else{chartMinValue-=Math.abs(chartMinValue*a.minValueScale-chartMinValue)}if(this.options.yRangeFunction){var c=this.options.yRangeFunction({min:chartMinValue,max:chartMaxValue});chartMinValue=c.min;chartMaxValue=c.max}if(!isNaN(chartMaxValue)&&!isNaN(chartMinValue)){var e=chartMaxValue-chartMinValue;var f=(e-this.currentValueRange);var g=(chartMinValue-this.currentVisMinValue);this.isAnimatingScale=Math.abs(f)>0.1||Math.abs(g)>0.1;this.currentValueRange+=a.scaleSmoothing*f;this.currentVisMinValue+=a.scaleSmoothing*g}this.valueRange={min:chartMinValue,max:chartMaxValue}};SmoothieChart.prototype.render=function(c,e){var f=new Date().getTime();if(!this.isAnimatingScale){var g=Math.min(1000/6,this.options.millisPerPixel);if(f-this.lastRenderTimeMillis<g){return}}this.resize();this.lastRenderTimeMillis=f;c=c||this.canvas;e=e||f-(this.delay||0);e-=e%this.options.millisPerPixel;var h=c.getContext('2d'),chartOptions=this.options,dimensions={top:0,left:0,width:c.clientWidth,height:c.clientHeight},oldestValidTime=e-(dimensions.width*chartOptions.millisPerPixel),valueToYPixel=function(a){var b=a-this.currentVisMinValue;return this.currentValueRange===0?dimensions.height:dimensions.height-(Math.round((b/this.currentValueRange)*dimensions.height))}.bind(this),timeToXPixel=function(t){if(chartOptions.scrollBackwards){return Math.round((e-t)/chartOptions.millisPerPixel)}return Math.round(dimensions.width-((e-t)/chartOptions.millisPerPixel))};this.updateValueRange();h.font=chartOptions.labels.fontSize+'px '+chartOptions.labels.fontFamily;h.save();h.translate(dimensions.left,dimensions.top);h.beginPath();h.rect(0,0,dimensions.width,dimensions.height);h.clip();h.save();h.fillStyle=chartOptions.grid.fillStyle;h.clearRect(0,0,dimensions.width,dimensions.height);h.fillRect(0,0,dimensions.width,dimensions.height);h.restore();h.save();h.lineWidth=chartOptions.grid.lineWidth;h.strokeStyle=chartOptions.grid.strokeStyle;if(chartOptions.grid.millisPerLine>0){h.beginPath();for(var t=e-(e%chartOptions.grid.millisPerLine);t>=oldestValidTime;t-=chartOptions.grid.millisPerLine){var j=timeToXPixel(t);if(chartOptions.grid.sharpLines){j-=0.5}h.moveTo(j,0);h.lineTo(j,dimensions.height)}h.stroke();h.closePath()}for(var v=1;v<chartOptions.grid.verticalSections;v++){var k=Math.round(v*dimensions.height/chartOptions.grid.verticalSections);if(chartOptions.grid.sharpLines){k-=0.5}h.beginPath();h.moveTo(0,k);h.lineTo(dimensions.width,k);h.stroke();h.closePath()}if(chartOptions.grid.borderVisible){h.beginPath();h.strokeRect(0,0,dimensions.width,dimensions.height);h.closePath()}h.restore();if(chartOptions.horizontalLines&&chartOptions.horizontalLines.length){for(var l=0;l<chartOptions.horizontalLines.length;l++){var m=chartOptions.horizontalLines[l],hly=Math.round(valueToYPixel(m.value))-0.5;h.strokeStyle=m.color||'#ffffff';h.lineWidth=m.lineWidth||1;h.beginPath();h.moveTo(0,hly);h.lineTo(dimensions.width,hly);h.stroke();h.closePath()}}for(var d=0;d<this.seriesSet.length;d++){h.save();var n=this.seriesSet[d].timeSeries,dataSet=n.data,seriesOptions=this.seriesSet[d].options;n.dropOldData(oldestValidTime,chartOptions.maxDataSetLength);h.lineWidth=seriesOptions.lineWidth;h.strokeStyle=seriesOptions.strokeStyle;h.beginPath();var o=0,lastX=0,lastY=0;for(var i=0;i<dataSet.length&&dataSet.length!==1;i++){var x=timeToXPixel(dataSet[i][0]),y=valueToYPixel(dataSet[i][1]);if(i===0){o=x;h.moveTo(x,y)}else{switch(chartOptions.interpolation){case\"linear\":case\"line\":{h.lineTo(x,y);break}case\"bezier\":default:{h.bezierCurveTo(Math.round((lastX+x)/2),lastY,Math.round((lastX+x))/2,y,x,y);break}case\"step\":{h.lineTo(x,lastY);h.lineTo(x,y);break}}}lastX=x;lastY=y}if(dataSet.length>1){if(seriesOptions.fillStyle){h.lineTo(dimensions.width+seriesOptions.lineWidth+1,lastY);h.lineTo(dimensions.width+seriesOptions.lineWidth+1,dimensions.height+seriesOptions.lineWidth+1);h.lineTo(o,dimensions.height+seriesOptions.lineWidth);h.fillStyle=seriesOptions.fillStyle;h.fill()}if(seriesOptions.strokeStyle&&seriesOptions.strokeStyle!=='none'){h.stroke()}h.closePath()}h.restore()}if(!chartOptions.labels.disabled&&!isNaN(this.valueRange.min)&&!isNaN(this.valueRange.max)){var p=chartOptions.yMaxFormatter(this.valueRange.max,chartOptions.labels.precision),minValueString=chartOptions.yMinFormatter(this.valueRange.min,chartOptions.labels.precision),maxLabelPos=chartOptions.scrollBackwards?0:dimensions.width-h.measureText(p).width-2,minLabelPos=chartOptions.scrollBackwards?0:dimensions.width-h.measureText(minValueString).width-2;h.fillStyle=chartOptions.labels.fillStyle;h.fillText(p,maxLabelPos,chartOptions.labels.fontSize);h.fillText(minValueString,minLabelPos,dimensions.height-2)}if(chartOptions.timestampFormatter&&chartOptions.grid.millisPerLine>0){var q=chartOptions.scrollBackwards?h.measureText(minValueString).width:dimensions.width-h.measureText(minValueString).width+4;for(var t=e-(e%chartOptions.grid.millisPerLine);t>=oldestValidTime;t-=chartOptions.grid.millisPerLine){var j=timeToXPixel(t);if((!chartOptions.scrollBackwards&&j<q)||(chartOptions.scrollBackwards&&j>q)){var r=new Date(t),ts=chartOptions.timestampFormatter(r),tsWidth=h.measureText(ts).width;q=chartOptions.scrollBackwards?j+tsWidth+2:j-tsWidth-2;h.fillStyle=chartOptions.labels.fillStyle;if(chartOptions.scrollBackwards){h.fillText(ts,j,dimensions.height-2)}else{h.fillText(ts,j-tsWidth,dimensions.height-2)}}}}h.restore()};SmoothieChart.timeFormatter=function(b){function pad2(a){return(a<10?'0':'')+a}return pad2(b.getHours())+':'+pad2(b.getMinutes())+':'+pad2(b.getSeconds())};s.TimeSeries=TimeSeries;s.SmoothieChart=SmoothieChart})(typeof exports==='undefined'?this:exports);";
