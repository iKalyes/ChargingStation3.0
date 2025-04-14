#include <web_server.h>

static AsyncWebServer server(80);
AsyncWebSocket ws("/ws"); // 创建WebSocket对象
unsigned long lastTime = 0;
const long interval = 100; // 数据更新间隔(毫秒)

// ===== HTML页面定义 =====
const char index_html[] PROGMEM = R"rawliteral(
    <!DOCTYPE HTML>
    <html>
    <head>
      <meta charset="UTF-8">
      <title>多协议桌面充电站</title>
      <meta name="viewport" content="width=device-width, initial-scale=1" id="viewport-meta">
      <script>
      // 强制手机使用桌面视图
      (function() {
        // 检测是否为移动设备
        function isMobileDevice() {
          return /Android|webOS|iPhone|iPad|iPod|BlackBerry|IEMobile|Opera Mini/i.test(navigator.userAgent)
            || (navigator.userAgent.includes("Mac") && "ontouchend" in document);
        }
        
        // 如果是移动设备，修改viewport
        if (isMobileDevice()) {
          var metaViewport = document.getElementById('viewport-meta');
          // 强制使用固定宽度，禁用缩放
          metaViewport.setAttribute('content', 'width=720, initial-scale=0.5, maximum-scale=0.5, user-scalable=0');
          
          // 添加额外CSS以解决触摸设备上的布局问题
          var style = document.createElement('style');
          style.innerHTML = `
            body { min-width: 720px; }
            .chart-group { 
              min-width: calc(33.33% - 10px); 
            }
            /* 启用触摸滚动 */
            html, body {
              overflow-x: auto;
              -webkit-overflow-scrolling: touch;
            }
          `;
          document.head.appendChild(style);
        }
      })();
      </script>
      <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
      <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
      <style>
        html {
          font-family: Arial, Helvetica, sans-serif;
          text-align: center;
        }
        h1 {
          font-size: 1.8rem;
          color: white;
          margin: 10px 0;             /* 调整上下边距 */
          text-align: left;           /* 文字左对齐 */
        }
        h2 {
          font-size: 0.85rem; /* 稍微减小标题字体从0.9rem到0.85rem */
          margin: 1px 0;      /* 减小上下边距 */
          text-align: left;   /* 标题左对齐 */
          display: flex;
          justify-content: space-between;
          align-items: center;
          padding: 0 5px;
        }
        h3 {
          font-size: 0.95rem;
          margin: 0;                /* 移除外边距 */
          padding: 6px 8px;         /* 增加内边距让文字更居中 */
          border-radius: 10px 10px 10px 10px;
          background-color: #3498db; /* 添加蓝色背景 */
          color: white;             /* 文字改为白色以增加对比度 */
          font-weight: bold;        /* 文字加粗 */
          text-align: center;       /* 文字居中 */
          box-shadow: 0 1px 3px rgba(0,0,0,0.1); /* 添加轻微阴影效果 */
          width: 100%;              /* 确保宽度与图表组相同 */
          box-sizing: border-box;   /* 确保内边距计入宽度 */
          margin-bottom: 6px;       /* 添加与下方内容的间距 */
        }
        .topnav {
          overflow: hidden;
          background-color: #143642;
          display: flex;              /* 使用flex布局 */
          justify-content: space-between; /* 两端对齐 */
          align-items: center;        /* 垂直居中 */
          padding: 0 15px;            /* 左右内边距 */
        }
        .nav-right {
          display: flex;
          align-items: center;
          gap: 15px;
        }
        .dark-mode-button {
          background: none;
          border: none;
          color: white;
          font-size: 1.2rem;
          cursor: pointer;
          padding: 8px;
          border-radius: 50%;
          width: 40px;
          height: 40px;
          display: flex;
          align-items: center;
          justify-content: center;
          background-color: rgba(255,255,255,0.1);
          transition: background-color 0.3s;
        }
        .dark-mode-button:hover {
          background-color: rgba(255,255,255,0.2);
        }
        /* 黑暗模式样式 */
        body.dark-mode {
          background-color: #121212;
          color: #e0e0e0;
        }
        body.dark-mode .chart-group,
        body.dark-mode .main-power-group,
        body.dark-mode .control-panel {
          background-color: #1e1e1e;
          border-color: #333;
        }
        body.dark-mode .card,
        body.dark-mode .horizontal-card {
          background-color: #2d2d2d;
          box-shadow: 1px 1px 6px 0px rgba(0,0,0,.5);
        }
        body.dark-mode h2 {
          color: #e0e0e0;
        }
        body.dark-mode .value-display {
          background-color: #3d3d3d;
          color: #4db8ff;
        }
        body.dark-mode .max-value {
          color: #aaa;
        }
        body.dark-mode .slider {
          background-color: #555;
        }
        body.dark-mode .switch-label {
          color: #ffffff; /* 黑暗模式下开关标签文字改为白色 */
        }
        body {
          margin: 0;
        }
        .content {
          padding: 10px;
          width: 100%;
          margin: 0 auto;
          display: flex;  /* 使用flex布局使图表组横向排列 */
          flex-wrap: wrap; /* 允许在小屏幕上换行 */
          box-sizing: border-box;
          justify-content: center; /* 居中对齐图表组 */
        }
        .chart-group {
          width: calc(33.33% - 10px);
          padding: 6px;
          box-sizing: border-box;
          border: 1px solid #ccc;
          border-radius: 10px;
          margin: 0 5px 8px 5px;
          background-color: #fafafa;
          overflow: hidden;        /* 确保内部元素不会溢出圆角边框 */
        }
        .card {
          background-color: white;
          box-shadow: 1px 1px 6px 0px rgba(140,140,140,.5);
          padding: 2px;  /* 减小内边距从3px到2px */
          margin-bottom: 4px;  /* 减小底部间距从5px到4px */
          border-radius: 10px;  /* 添加圆角 */
        }
        .chart-container {
          width: 100%;
          height: 100px;  /* 减小高度从150px到100px */
          margin: 2px auto;
        }
        .value-display {
          display: inline-block;
          min-width: 60px;
          text-align: center;       /* 数值居中显示 */
          font-weight: bold;
          color: #0066cc;
          background-color: #f0f8ff;
          padding: 2px 6px;
          border-radius: 4px;
          margin: 0 8px;            /* 左右均添加间距 */
        }
        .max-value {
          font-size: 0.75rem;       /* 最大值字体稍小 */
          color: #666;
          font-weight: normal;
          white-space: nowrap;      /* 防止换行 */
        }
        .main-power-group {
          width: calc(100% - 10px);
          padding: 6px;
          box-sizing: border-box;
          border: 1px solid #ccc;
          border-radius: 10px;
          margin: 0 5px 16px 5px; /* 增加底部外边距 */
          background-color: #fafafa;
          overflow: hidden;
        }
        .horizontal-charts {
          display: flex;
          justify-content: space-between;
          gap: 8px; /* 添加间距 */
        }
        .horizontal-card {
          width: calc(33% - 8px); /* 减小宽度从33.33%到33%并考虑间距 */
          background-color: white;
          box-shadow: 1px 1px 6px 0px rgba(140,140,140,.5);
          padding: 2px;
          border-radius: 10px;
        }
        .control-panel {
          width: calc(100% - 10px);
          padding: 6px;
          box-sizing: border-box;
          border: 1px solid #ccc;
          border-radius: 10px;
          margin: 0 5px 16px 5px;
          background-color: #fafafa;
          overflow: hidden;
        }
        .switches-container {
          display: flex;
          flex-direction: column; /* 改为列方向，支持多行排列 */
          gap: 10px; /* 行间距 */
          padding: 10px;
        }
        .switch-row {
          display: flex;
          flex-wrap: wrap;
          justify-content: space-between; /* 均匀分布 */
          width: 100%;
        }
        .switch-item {
          flex: 0 0 calc(33.33% - 20px);
          max-width: calc(33.33% - 20px);
          display: flex;
          align-items: center;
          margin: 8px;
        }
        .switch-item.empty {
          visibility: hidden;
        }
        .switch-label {
          margin-left: 10px;
          font-size: 0.95rem;
          font-weight: 500;
          color: #333;
        }
        .switch {
          position: relative;
          display: inline-block;
          width: 60px;
          height: 34px;
        }
        .switch input {
          opacity: 0;
          width: 0;
          height: 0;
        }
        .slider {
          position: absolute;
          cursor: pointer;
          top: 0;
          left: 0;
          right: 0;
          bottom: 0;
          background-color: #ccc;
          transition: .4s;
        }
        .slider:before {
          position: absolute;
          content: "";
          height: 26px;
          width: 26px;
          left: 4px;
          bottom: 4px;
          background-color: white;
          transition: .4s;
        }
        input:checked + .slider {
          background-color: #2196F3;
        }
        input:focus + .slider {
          box-shadow: 0 0 1px #2196F3;
        }
        input:checked + .slider:before {
          transform: translateX(26px);
        }
        .slider.round {
          border-radius: 34px;
        }
        .slider.round:before {
          border-radius: 50%;
        }
        .time-display {
          font-size: 1.8rem; /* 与标题h1相同的字体大小 */
          color: white;
          background-color: rgba(255,255,255,0.1); /* 半透明背景 */
          padding: 5px 15px;
          border-radius: 8px;
          font-weight: 500;
          box-shadow: 0 1px 3px rgba(0,0,0,0.2);
          min-width: 120px; /* 保证宽度足够 */
          text-align: center;
        }
      </style>
    </head>
    <body>
      <div class="topnav">
        <h1>多协议桌面充电站</h1>
        <div class="nav-right">
          <button id="darkModeToggle" class="dark-mode-button">
            <i class="fas fa-moon"></i>
          </button>
          <div id="currentTime" class="time-display">00:00:00</div>
        </div>
      </div>
      <div class="content">
        <!-- 主电源输入图表组 -->
        <div class="main-power-group">
          <h3>主电源输入</h3>
          <div class="horizontal-charts">
            <div class="horizontal-card">
              <h2>电压[V]<span id="voltageValueMain" class="value-display">0.00 V</span></h2>
              <div class="chart-container">
                <canvas id="voltageChartMain"></canvas>
              </div>
            </div>
            <div class="horizontal-card">
              <h2>电流[A]<span id="currentValueMain" class="value-display">0.00 A</span></h2>
              <div class="chart-container">
                <canvas id="currentChartMain"></canvas>
              </div>
            </div>
            <div class="horizontal-card">
              <h2>功率[W]<span id="powerValueMain" class="value-display">0.00 W</span></h2>
              <div class="chart-container">
                <canvas id="powerChartMain"></canvas>
              </div>
            </div>
          </div>
        </div>
        
        <!-- 在主电源输入图表组后添加控制面板 -->
        <div class="control-panel">
          <h3>系统控制</h3>
          <div class="switches-container">
            <!-- 第一行：Type-C 3&2输出，Type-C 1输出，USB-A输出 -->
            <div class="switch-row">
              <div class="switch-item">
                <label class="switch">
                  <input type="checkbox" id="typeC32-switch" checked>
                  <span class="slider round"></span>
                </label>
                <span class="switch-label">Type-C输出[3][2]</span>
              </div>
              <div class="switch-item">
                <label class="switch">
                  <input type="checkbox" id="typeC1-switch" checked>
                  <span class="slider round"></span>
                </label>
                <span class="switch-label">Type-C输出[1]</span>
              </div>
              <div class="switch-item">
                <label class="switch">
                  <input type="checkbox" id="usbA-switch" checked>
                  <span class="slider round"></span>
                </label>
                <span class="switch-label">USB-A输出[4][5]</span>
              </div>
            </div>
            
            <!-- 第二行：散热风扇、风扇温控和充电指示灯 -->
            <div class="switch-row">
              <div class="switch-item">
                <label class="switch">
                  <input type="checkbox" id="fan-switch" checked>
                  <span class="slider round"></span>
                </label>
                <span class="switch-label">散热风扇开关</span>
              </div>
              <div class="switch-item">
                <label class="switch">
                  <input type="checkbox" id="fan-temp-control" checked>
                  <span class="slider round"></span>
                </label>
                <span class="switch-label">风扇温控开关</span>
              </div>
              <div class="switch-item">
                <label class="switch">
                  <input type="checkbox" id="charge-led" checked>
                  <span class="slider round"></span>
                </label>
                <span class="switch-label">充电指示灯开关</span>
              </div>
            </div>
          </div>
        </div>
        
        <!-- 第一组图表 -->
        <div class="chart-group">
          <h3>Type-C 输出[3]</h3>
          <div class="card">
            <h2>电压[V]<span id="voltageValue1" class="value-display">0.00 V</span></h2>
            <div class="chart-container">
              <canvas id="voltageChart1"></canvas>
            </div>
          </div>
          <div class="card">
            <h2>电流[A]<span id="currentValue1" class="value-display">0.00 A</span></h2>
            <div class="chart-container">
              <canvas id="currentChart1"></canvas>
            </div>
          </div>
          <div class="card">
            <h2>功率[W]<span id="powerValue1" class="value-display">0.00 W</span></h2>
            <div class="chart-container">
              <canvas id="powerChart1"></canvas>
            </div>
          </div>
        </div>
        
        <!-- 第二组图表 -->
        <div class="chart-group">
          <h3>Type-C 输出[2]</h3>
          <div class="card">
            <h2>电压[V]<span id="voltageValue2" class="value-display">0.00 V</span></h2>
            <div class="chart-container">
              <canvas id="voltageChart2"></canvas>
            </div>
          </div>
          <div class="card">
            <h2>电流[A]<span id="currentValue2" class="value-display">0.00 A</span></h2>
            <div class="chart-container">
              <canvas id="currentChart2"></canvas>
            </div>
          </div>
          <div class="card">
            <h2>功率[W]<span id="powerValue2" class="value-display">0.00 W</span></h2>
            <div class="chart-container">
              <canvas id="powerChart2"></canvas>
            </div>
          </div>
        </div>
        
        <!-- 第三组图表 -->
        <div class="chart-group">
          <h3>Type-C 输出[1]</h3>
          <div class="card">
            <h2>电压[W]<span id="voltageValue3" class="value-display">0.00 V</span></h2>
            <div class="chart-container">
              <canvas id="voltageChart3"></canvas>
            </div>
          </div>
          <div class="card">
            <h2>电流[A]<span id="currentValue3" class="value-display">0.00 A</span></h2>
            <div class="chart-container">
              <canvas id="currentChart3"></canvas>
            </div>
          </div>
          <div class="card">
            <h2>功率[W]<span id="powerValue3" class="value-display">0.00 W</span></h2>
            <div class="chart-container">
              <canvas id="powerChart3"></canvas>
            </div>
          </div>
        </div>
    
        <!-- USB-A OUT4 图表 -->
        <div class="chart-group">
          <h3>USB-A 输出[4]</h3>
          <div class="card">
            <h2>电压[V]<span id="voltageValue4" class="value-display">0.00 V</span></h2>
            <div class="chart-container">
              <canvas id="voltageChart4"></canvas>
            </div>
          </div>
        </div>
    
        <!-- USB-A OUT5 图表 -->
        <div class="chart-group">
          <h3>USB-A 输出[5]</h3>
          <div class="card">
            <h2>电压[V]<span id="voltageValue5" class="value-display">0.00 V</span></h2>
            <div class="chart-container">
              <canvas id="voltageChart5"></canvas>
            </div>
          </div>
        </div>
    
        <!-- 系统温度图表 -->
        <div class="chart-group">
          <h3>系统温度</h3>
          <div class="card">
            <h2>温度[°C]<span id="tempValue" class="value-display">0.0 °C</span></h2>
            <div class="chart-container">
              <canvas id="tempChart"></canvas>
            </div>
          </div>
        </div>
      </div>
      <script>
        // 配置WebSocket连接
        var gateway = `ws://${window.location.hostname}/ws`;
        var websocket;
        
        // 更新时间函数
        function updateClock() {
          const now = new Date();
          const hours = String(now.getHours()).padStart(2, '0');
          const minutes = String(now.getMinutes()).padStart(2, '0');
          const seconds = String(now.getSeconds()).padStart(2, '0');
          document.getElementById('currentTime').textContent = `${hours}:${minutes}:${seconds}`;
        }
    
        // 修改createChartOptions函数，优化高功率显示
        function createChartOptions(maxValue, unit) {
          // 根据单位和最大值动态调整刻度设置
          let tickConfig = {};
          
          if (unit === 'V') {
            // 电压刻度配置
            if (maxValue <= 15) {
              tickConfig = {
                stepSize: 3,  // 每3V一个刻度
                count: 6      // 最多显示6个刻度点
              };
            } else {
              tickConfig = {
                stepSize: 5,  // 每5V一个刻度
                count: 6      // 最多显示6个刻度点
              };
            }
          } else if (unit === 'A') {
            // 电流刻度配置
            if (maxValue <= 5) {
              tickConfig = {
                stepSize: 1,  // 每1A一个刻度
                count: 6      // 最多显示6个刻度点
              };
            } else {
              tickConfig = {
                stepSize: 2,  // 每2A一个刻度
                count: 5      // 最多显示5个刻度点
              };
            }
          } else if (unit === 'W') {
            // 功率刻度配置
            if (maxValue <= 35) {
              tickConfig = {
                stepSize: 7,  // 每7W一个刻度
                count: 6      // 最多显示6个刻度点
              };
            } else if (maxValue <= 125) {
              tickConfig = {
                stepSize: 25, // 每25W一个刻度
                count: 6      // 最多显示6个刻度点
              };
            } else {
              // 特殊处理大功率范围(240W)
              tickConfig = {
                stepSize: 40, // 每40W一个刻度
                count: 6      // 最多显示6个刻度点
              };
            }
          } else if (unit === '°C') {
            // 温度刻度配置
            tickConfig = {
              stepSize: 10,  // 每10°C一个刻度
              count: 7       // 最多显示7个刻度点
            };
          }
          
          // 创建图表配置选项
          return {
            scales: {
              y: {
                beginAtZero: true,
                max: maxValue,
                title: {
                  display: false,  // 不显示y轴单位
                  text: unit,
                  font: {
                    size: 10
                  }
                },
                ticks: {
                  font: {
                    size: 8  // 较小的Y轴标签字体
                  },
                  stepSize: tickConfig.stepSize, // 使用固定步长
                  padding: 5,       // 增加标签内边距
                  callback: function(value) {
                    // 对大数值使用更紧凑的格式
                    if (value === 0) return '0';
                    if (unit === 'W' && value >= 100) {
                      // 功率值大于等于100W时不显示小数位
                      return Math.round(value);
                    } else if (value < 10) {
                      return value.toFixed(1);
                    } else {
                      return Math.round(value);
                    }
                  }
                }
              },
              x: {
                display: false,  // 不显示x轴
                ticks: {
                  font: {
                    size: 9
                  }
                }
              }
            },
            animation: {
              duration: 0
            },
            elements: {
              point: {
                radius: 0,
                hitRadius: 5,
                hoverRadius: 3
              }
            },
            plugins: {
              legend: {
                display: false  // 隐藏图例以节省空间
              },
              tooltip: {
                callbacks: {
                  label: function(context) {
                    // 添加单位到显示值
                    return context.parsed.y + ' ' + unit;
                  }
                }
              }
            },
            responsive: true,
            maintainAspectRatio: false,
            layout: {
              padding: {
                left: 15,    // 增加左侧内边距，为大数值Y轴刻度提供更多空间
                right: 5,    // 保持右侧内边距
                top: 5,      // 顶部边距
                bottom: 5    // 底部边距
              }
            }
          };
        }
    
        // 初始化所有图表数据数组
        var chartGroups = [];
        
        // 为3个图表组创建数据对象
        for (let group = 1; group <= 3; group++) {
          chartGroups[group] = {
            voltage: {
              chart: null,
              data: {
                labels: Array(100).fill(""),
                datasets: [{
                  label: `电压 (V) - 组${group}`,
                  borderColor: 'rgb(255, 99, 132)',  // 红色
                  borderWidth: 2,
                  data: Array(100).fill(0),
                  fill: false,
                  tension: 0.4
                }]
              }
            },
            current: {
              chart: null,
              data: {
                labels: Array(100).fill(""),
                datasets: [{
                  label: `电流 (A) - 组${group}`,
                  borderColor: 'rgb(75, 192, 192)',  // 绿色
                  borderWidth: 2,
                  data: Array(100).fill(0),
                  fill: false,
                  tension: 0.4
                }]
              }
            },
            power: {
              chart: null,
              data: {
                labels: Array(100).fill(""),
                datasets: [{
                  label: `功率 (W) - 组${group}`,
                  borderColor: 'rgb(54, 162, 235)',  // 蓝色
                  borderWidth: 2,
                  data: Array(100).fill(0),
                  fill: false,
                  tension: 0.4
                }]
              }
            }
          };
        }
    
        // 初始化主电源输入图表组数据
        var mainPowerGroup = {
          voltage: {
            chart: null,
            data: {
              labels: Array(100).fill(""),
              datasets: [{
                label: '电压 (V) - 主电源',
                borderColor: 'rgb(255, 99, 132)',  // 红色
                borderWidth: 2,
                data: Array(100).fill(0),
                fill: false,
                tension: 0.4
              }]
            }
          },
          current: {
            chart: null,
            data: {
              labels: Array(100).fill(""),
              datasets: [{
                label: '电流 (A) - 主电源',
                borderColor: 'rgb(75, 192, 192)',  // 绿色
                borderWidth: 2,
                data: Array(100).fill(0),
                fill: false,
                tension: 0.4
              }]
            }
          },
          power: {
            chart: null,
            data: {
              labels: Array(100).fill(""),
              datasets: [{
                label: '功率 (W) - 主电源',
                borderColor: 'rgb(54, 162, 235)',  // 蓝色
                borderWidth: 2,
                data: Array(100).fill(0),
                fill: false,
                tension: 0.4
              }]
            }
          }
        };
    
        // 添加USB-A OUT4电压图表数据
        var usbA4Chart = {
          voltage: {
            chart: null,
            data: {
              labels: Array(100).fill(""),
              datasets: [{
                label: '电压 (V) - USB-A OUT4',
                borderColor: 'rgb(255, 99, 132)',  // 红色
                borderWidth: 2,
                data: Array(100).fill(0),
                fill: false,
                tension: 0.4
              }]
            }
          }
        };
    
        // 添加USB-A OUT5电压图表数据
        var usbA5Chart = {
          voltage: {
            chart: null,
            data: {
              labels: Array(100).fill(""),
              datasets: [{
                label: '电压 (V) - USB-A OUT5',
                borderColor: 'rgb(255, 99, 132)',  // 红色
                borderWidth: 2,
                data: Array(100).fill(0),
                fill: false,
                tension: 0.4
              }]
            }
          }
        };
    
        // 添加系统温度图表数据
        var tempChart = {
          chart: null,
          data: {
            labels: Array(100).fill(""),
            datasets: [{
              label: '温度 (°C)',
              borderColor: 'rgb(255, 159, 64)',  // 橙色
              borderWidth: 2,
              data: Array(100).fill(0),
              fill: false,
              tension: 0.4
            }]
          }
        };
    
        // 连接WebSocket
        function initWebSocket() {
          console.log('正在尝试打开WebSocket连接...');
          websocket = new WebSocket(gateway);
          websocket.onopen = onOpen;
          websocket.onclose = onClose;
          websocket.onmessage = onMessage;
        }
    
        // 在WebSocket连接成功后请求初始开关状态
        function onOpen(event) {
          console.log('WebSocket连接已打开');
          // 连接后立即发送请求获取开关状态
          // (注：此处已不需要手动请求，服务器会自动发送)
        }
    
        function onClose(event) {
          console.log('WebSocket连接已关闭');
          setTimeout(initWebSocket, 2000);
        }
    
        // 修改onMessage函数处理新图表数据
    
        function onMessage(event) {
          var data = JSON.parse(event.data);
          
          // 处理开关状态更新
          if (data.switches) {
            updateSwitchStates(data);
          }
          
          // 处理主电源输入数据
          let mainPowerData = data["mainPower"];
          if (mainPowerData) {
            // 更新电压值显示和图表
            document.getElementById("voltageValueMain").textContent = mainPowerData.voltage.toFixed(3) + " V";
            mainPowerGroup.voltage.data.datasets[0].data.shift();
            mainPowerGroup.voltage.data.datasets[0].data.push(mainPowerData.voltage);
            mainPowerGroup.voltage.data.labels.shift();
            mainPowerGroup.voltage.data.labels.push(data.time);
            mainPowerGroup.voltage.chart.update();
            
            // 更新电流值显示和图表
            document.getElementById("currentValueMain").textContent = mainPowerData.current.toFixed(3) + " A";
            mainPowerGroup.current.data.datasets[0].data.shift();
            mainPowerGroup.current.data.datasets[0].data.push(mainPowerData.current);
            mainPowerGroup.current.data.labels.shift();
            mainPowerGroup.current.data.labels.push(data.time);
            mainPowerGroup.current.chart.update();
            
            // 更新功率值显示和图表
            document.getElementById("powerValueMain").textContent = mainPowerData.power.toFixed(2) + " W";
            mainPowerGroup.power.data.datasets[0].data.shift();
            mainPowerGroup.power.data.datasets[0].data.push(mainPowerData.power);
            mainPowerGroup.power.data.labels.shift();
            mainPowerGroup.power.data.labels.push(data.time);
            mainPowerGroup.power.chart.update();
          }
          
          // 更新所有图表组的数据，每组使用独立的数据
          for (let group = 1; group <= 3; group++) {
            let groupData = data["group" + group]; // 获取当前图表组的数据
            
            if (groupData) {
              // 更新电压值显示和图表
              document.getElementById(`voltageValue${group}`).textContent = groupData.voltage.toFixed(3) + " V";
              chartGroups[group].voltage.data.datasets[0].data.shift();
              chartGroups[group].voltage.data.datasets[0].data.push(groupData.voltage);
              chartGroups[group].voltage.data.labels.shift();
              chartGroups[group].voltage.data.labels.push(data.time);
              chartGroups[group].voltage.chart.update();
              
              // 更新电流值显示和图表
              document.getElementById(`currentValue${group}`).textContent = groupData.current.toFixed(3) + " A";
              chartGroups[group].current.data.datasets[0].data.shift();
              chartGroups[group].current.data.datasets[0].data.push(groupData.current);
              chartGroups[group].current.data.labels.shift();
              chartGroups[group].current.data.labels.push(data.time);
              chartGroups[group].current.chart.update();
              
              // 更新功率值显示和图表
              document.getElementById(`powerValue${group}`).textContent = groupData.power.toFixed(2) + " W";
              chartGroups[group].power.data.datasets[0].data.shift();
              chartGroups[group].power.data.datasets[0].data.push(groupData.power);
              chartGroups[group].power.data.labels.shift();
              chartGroups[group].power.data.labels.push(data.time);
              chartGroups[group].power.chart.update();
            }
          }
          
          // 处理USB-A OUT4电压数据
          let usbA4Data = data["usbA4"];
          if (usbA4Data) {
            document.getElementById("voltageValue4").textContent = usbA4Data.voltage.toFixed(2) + " V";
            usbA4Chart.voltage.data.datasets[0].data.shift();
            usbA4Chart.voltage.data.datasets[0].data.push(usbA4Data.voltage);
            usbA4Chart.voltage.data.labels.shift();
            usbA4Chart.voltage.data.labels.push(data.time);
            usbA4Chart.voltage.chart.update();
          }
          
          // 处理USB-A OUT5电压数据
          let usbA5Data = data["usbA5"];
          if (usbA5Data) {
            document.getElementById("voltageValue5").textContent = usbA5Data.voltage.toFixed(2) + " V";
            usbA5Chart.voltage.data.datasets[0].data.shift();
            usbA5Chart.voltage.data.datasets[0].data.push(usbA5Data.voltage);
            usbA5Chart.voltage.data.labels.shift();
            usbA5Chart.voltage.data.labels.push(data.time);
            usbA5Chart.voltage.chart.update();
          }
          
          // 处理系统温度数据
          let tempData = data["systemTemp"];
          if (tempData) {
            document.getElementById("tempValue").textContent = tempData.toFixed(2) + " °C";
            tempChart.data.datasets[0].data.shift();
            tempChart.data.datasets[0].data.push(tempData);
            tempChart.data.labels.shift();
            tempChart.data.labels.push(data.time);
            tempChart.chart.update();
          }
        }
    
        // 修改updateSwitchStates函数
        function updateSwitchStates(data) {
          if (data.switches) {
            // 更新开关UI状态
            if (data.switches.typeC32 !== undefined) {
              document.getElementById('typeC32-switch').checked = data.switches.typeC32;
            }
            if (data.switches.typeC1 !== undefined) {
              document.getElementById('typeC1-switch').checked = data.switches.typeC1;
            }
            if (data.switches.usbA !== undefined) {
              document.getElementById('usbA-switch').checked = data.switches.usbA;
            }
            if (data.switches.fan !== undefined) {
              document.getElementById('fan-switch').checked = data.switches.fan;
            }
            if (data.switches.fanTemp !== undefined) {
              document.getElementById('fan-temp-control').checked = data.switches.fanTemp;
            }
            if (data.switches.chargeLed !== undefined) {
              document.getElementById('charge-led').checked = data.switches.chargeLed;
            }
          }
        }

        // 初始化所有图表
        function initCharts() {
          // 初始化主电源输入图表
          mainPowerGroup.voltage.chart = new Chart(
            document.getElementById("voltageChartMain").getContext('2d'), {
              type: 'line',
              data: mainPowerGroup.voltage.data,
              options: createChartOptions(30, 'V')  // 30V上限
            }
          );
          
          mainPowerGroup.current.chart = new Chart(
            document.getElementById("currentChartMain").getContext('2d'), {
              type: 'line',
              data: mainPowerGroup.current.data,
              options: createChartOptions(20, 'A')  // 20A上限
            }
          );
          
          mainPowerGroup.power.chart = new Chart(
            document.getElementById("powerChartMain").getContext('2d'), {
              type: 'line',
              data: mainPowerGroup.power.data,
              options: createChartOptions(240, 'W')  // 240W上限
            }
          );
    
          for (let group = 1; group <= 3; group++) {
            // 初始化电压图表 - 第一组改为15V，其他保持25V
            const voltageMax = group === 1 ? 15 : 25;
            chartGroups[group].voltage.chart = new Chart(
              document.getElementById(`voltageChart${group}`).getContext('2d'), {
                type: 'line',
                data: chartGroups[group].voltage.data,
                options: createChartOptions(voltageMax, 'V')
              }
            );
            
            // 初始化电流图表 - 第一组改为5A，其他改为8A
            const currentMax = group === 1 ? 5 : 8;
            chartGroups[group].current.chart = new Chart(
              document.getElementById(`currentChart${group}`).getContext('2d'), {
                type: 'line',
                data: chartGroups[group].current.data,
                options: createChartOptions(currentMax, 'A')
              }
            );
            
            // 初始化功率图表 - 第一组改为35W，其他保持125W
            const powerMax = group === 1 ? 35 : 125;
            chartGroups[group].power.chart = new Chart(
              document.getElementById(`powerChart${group}`).getContext('2d'), {
                type: 'line',
                data: chartGroups[group].power.data,
                options: createChartOptions(powerMax, 'W')
              }
            );
          }
    
          // 初始化USB-A OUT4电压图表
          usbA4Chart.voltage.chart = new Chart(
            document.getElementById("voltageChart4").getContext('2d'), {
              type: 'line',
              data: usbA4Chart.voltage.data,
              options: createChartOptions(15, 'V')  // 15V上限
            }
          );
          
          // 初始化USB-A OUT5电压图表
          usbA5Chart.voltage.chart = new Chart(
            document.getElementById("voltageChart5").getContext('2d'), {
              type: 'line',
              data: usbA5Chart.voltage.data,
              options: createChartOptions(15, 'V')  // 15V上限
            }
          );
          
          // 初始化系统温度图表
          tempChart.chart = new Chart(
            document.getElementById("tempChart").getContext('2d'), {
              type: 'line',
              data: tempChart.data,
              options: createChartOptions(60, '°C')  // 60°C上限
            }
          );
        }

        // 切换黑暗模式
        function toggleDarkMode() {
          const body = document.body;
          const isDarkMode = body.classList.toggle('dark-mode');
          const darkModeButton = document.getElementById('darkModeToggle');
          
          // 更新图标
          if (isDarkMode) {
            darkModeButton.innerHTML = '<i class="fas fa-sun"></i>';
          } else {
            darkModeButton.innerHTML = '<i class="fas fa-moon"></i>';
          }
          
          // 保存偏好设置
          localStorage.setItem('darkMode', isDarkMode ? 'enabled' : 'disabled');
          
          // 更新图表主题颜色
          updateChartsTheme(isDarkMode);
        }

        // 更新图表主题颜色
        function updateChartsTheme(isDarkMode) {
          const gridColor = isDarkMode ? 'rgba(255, 255, 255, 0.1)' : 'rgba(0, 0, 0, 0.1)';
          const textColor = isDarkMode ? '#e0e0e0' : '#666';
          
          // 更新所有图表的主题
          const charts = [
            mainPowerGroup.voltage.chart, mainPowerGroup.current.chart, mainPowerGroup.power.chart,
            ...Object.values(chartGroups).flatMap(group => [group.voltage.chart, group.current.chart, group.power.chart]),
            usbA4Chart.voltage.chart, usbA5Chart.voltage.chart, tempChart.chart
          ];
          
          charts.forEach(chart => {
            if (chart) {
              chart.options.scales.y.grid.color = gridColor;
              chart.options.scales.y.ticks.color = textColor;
              chart.update();
            }
          });
        }

        // 初始化主题
        function initTheme() {
          const savedTheme = localStorage.getItem('darkMode');
          if (savedTheme === 'enabled') {
            document.body.classList.add('dark-mode');
            document.getElementById('darkModeToggle').innerHTML = '<i class="fas fa-sun"></i>';
            updateChartsTheme(true);
          }
        }
    
        // 在页面加载后，初始化开关事件监听器
        function initSwitches() {
          // 获取所有开关元素
          const typeC32Switch = document.getElementById('typeC32-switch');
          const typeC1Switch = document.getElementById('typeC1-switch');
          const usbASwitch = document.getElementById('usbA-switch');
          const fanSwitch = document.getElementById('fan-switch');
          const fanTempControl = document.getElementById('fan-temp-control');
          const chargeLed = document.getElementById('charge-led');
          
          // 添加事件监听器
          typeC32Switch.addEventListener('change', function() {
            sendSwitchState('typeC32', this.checked);
          });
          
          typeC1Switch.addEventListener('change', function() {
            sendSwitchState('typeC1', this.checked);
          });
          
          usbASwitch.addEventListener('change', function() {
            sendSwitchState('usbA', this.checked);
          });
          
          fanSwitch.addEventListener('change', function() {
            sendSwitchState('fan', this.checked);
          });
          
          fanTempControl.addEventListener('change', function() {
            sendSwitchState('fanTemp', this.checked);
          });
          
          chargeLed.addEventListener('change', function() {
            sendSwitchState('chargeLed', this.checked);
          });
        }
    
        // 发送开关状态到服务器
        function sendSwitchState(switchType, isOn) {
          if (websocket.readyState === WebSocket.OPEN) {
            const data = {
              action: 'switch',
              type: switchType,
              state: isOn
            };
            websocket.send(JSON.stringify(data));
            console.log(`发送开关状态: ${switchType} = ${isOn}`);
          } else {
            console.log('WebSocket未连接，无法发送开关状态');
          }
        }
    
        // 页面加载时初始化
        window.addEventListener('load', function() {
          // 初始化时间并设置定时更新
          updateClock();
          setInterval(updateClock, 1000);

          // 初始化主题
          initTheme();
  
          // 添加黑暗模式切换按钮事件
          document.getElementById('darkModeToggle').addEventListener('click', toggleDarkMode);
          
          // 图表初始化
          initCharts();
          
          // 开关初始化
          initSwitches();
          
          // WebSocket初始化
          initWebSocket();

          // 初始化主题
          initTheme();

          // 黑暗模式按钮事件
          document.getElementById('darkModeToggle').addEventListener('click', toggleDarkMode);
        });
      </script>
    </body>
    </html>
    )rawliteral";
    
    // ===== 开关控制函数 =====
    // Type-C32输出控制
    void setTypeC32Output(bool enabled) {
      USBC32_Switch = enabled;
      Serial.print("Type-C32输出: ");
      Serial.println(enabled ? "开启" : "关闭");
      if(enabled == true)
      {
        USBC32_ON();
        lv_obj_add_state(ui_USBC32Switch, LV_STATE_CHECKED);
        save_USBC_setting();
      }
      else
      {
        USBC32_OFF();
        lv_obj_clear_state(ui_USBC32Switch, LV_STATE_CHECKED);
        save_USBC_setting();
      }
    }

    // Type-C1输出控制
    void setTypeC1Output(bool enabled) {
      USBC1_Switch = enabled;
      Serial.print("Type-C1输出: ");
      Serial.println(enabled ? "开启" : "关闭");
      if(enabled == true)
      {
        USBC1_ON();
        lv_obj_add_state(ui_USBC1Switch, LV_STATE_CHECKED);
        save_USBC_setting();
      }
      else
      {
        USBC1_OFF();
        lv_obj_clear_state(ui_USBC1Switch, LV_STATE_CHECKED);
        save_USBC_setting();
      }
    }
    
    // USB-A输出控制
    void setUSBAOutput(bool enabled) {
      USBA_Switch = enabled;
      Serial.print("USB-A输出: ");
      Serial.println(enabled ? "开启" : "关闭");
      if(enabled == true)
      {
        USBA_ON();
        lv_obj_add_state(ui_USBASwitch, LV_STATE_CHECKED);
        save_USBA_setting();
      }
      else
      {
        USBA_OFF();
        lv_obj_clear_state(ui_USBASwitch, LV_STATE_CHECKED);
        save_USBA_setting();
      }
    }
    
    // 散热风扇控制
    void setFan(bool enabled) {
      fan_switch = enabled;
      Serial.print("散热风扇: ");
      Serial.println(enabled ? "开启" : "关闭");
      if(enabled == true)
      {
        lv_obj_add_state(ui_FanSwitch, LV_STATE_CHECKED);
        FAN_ON();
        save_setting();
      }
      else
      {
        lv_obj_clear_state(ui_FanSwitch, LV_STATE_CHECKED);
        FAN_OFF();
        save_setting();
      }
    }
    
    // 风扇温控功能
    void setFanTempControl(bool enabled) {
      tempcontrol_fan = enabled;
      Serial.print("风扇温控: ");
      Serial.println(enabled ? "开启" : "关闭");
      if(enabled == true)
      {
        lv_obj_add_state(ui_ThermometerControl, LV_STATE_CHECKED);
        tempcontrol_fan = true;
        save_advanced_setting();
      }
      else
      {
        lv_obj_clear_state(ui_ThermometerControl, LV_STATE_CHECKED);
        tempcontrol_fan = false;
        save_advanced_setting();
      }
    }
    
    // 充电指示灯控制
    void setChargeLed(bool enabled) {
      led_enabled = enabled;
      Serial.print("充电指示灯: ");
      Serial.println(enabled ? "开启" : "关闭");
      if(enabled == true)
      {
        lv_obj_add_state(ui_LEDControl, LV_STATE_CHECKED);
        led_enabled = true;
        save_advanced_setting();
      }
      else
      {
        lv_obj_clear_state(ui_LEDControl, LV_STATE_CHECKED);
        led_enabled = false;
        save_advanced_setting();
      }
    }
    
    // 发送所有开关状态（可指定特定客户端或发送给所有客户端）
    void sendSwitchStates(AsyncWebSocketClient *client) {
      // 创建包含开关状态的JSON文档
      JsonDocument doc;
      
      JsonObject switches = doc["switches"].to<JsonObject>();
      switches["typeC32"] = USBC32_Switch;
      switches["typeC1"] = USBC1_Switch;
      switches["usbA"] = USBA_Switch;
      switches["fan"] = fan_switch;
      switches["fanTemp"] = tempcontrol_fan;
      switches["chargeLed"] = led_enabled;
      
      // 将JSON文档序列化为字符串
      String jsonString;
      serializeJson(doc, jsonString);
      
      // 如果指定了客户端，则只发送给该客户端
      // 否则发送给所有连接的客户端
      if (client) {
        client->text(jsonString);
        Serial.printf("发送开关状态到客户端 #%u\n", client->id());
      } else if (ws.count() > 0) {
        ws.textAll(jsonString);
        Serial.println("发送开关状态到所有客户端");
      }
    }
    
    // ===== 开关状态控制函数 =====
    
    // 切换指定开关的状态（开->关 或 关->开）
    void toggleSwitch(const String& switchType) {
      if (switchType == "typeC32") {
        setTypeC32Output(!USBC32_Switch);
      } 
      else if( switchType == "typeC1") {
        setTypeC1Output(!USBC1_Switch);
      }
      else if (switchType == "usbA") {
        setUSBAOutput(!USBA_Switch);
      } 
      else if (switchType == "fan") {
        setFan(!fan_switch);
      } 
      else if (switchType == "fanTemp") {
        setFanTempControl(!tempcontrol_fan);
      } 
      else if (switchType == "chargeLed") {
        setChargeLed(!led_enabled);
      }
      
      // 发送更新后的状态
      sendSwitchStates();
    }
    
    // 直接设置指定开关为特定状态
    void setSwitch(const String& switchType, bool state) {
      if (switchType == "typeC32") {
        setTypeC32Output(state);
      } 
      else if (switchType == "typeC1") {
        setTypeC1Output(state);
      }
      else if (switchType == "usbA") {
        setUSBAOutput(state);
      } 
      else if (switchType == "fan") {
        setFan(state);
      } 
      else if (switchType == "fanTemp") {
        setFanTempControl(state);
      } 
      else if (switchType == "chargeLed") {
        setChargeLed(state);
      }
      
      // 发送更新后的状态
      sendSwitchStates();
    }
    
    //切换某个开关状态：
    //toggleSwitch("typeC");    // 切换Type-C输出开关状态
    //toggleSwitch("usbA");     // 切换USB-A输出开关状态
    //设置某个开关为特定状态：
    //setSwitch("fan", true);   // 打开散热风扇
    //setSwitch("fanTemp", false); // 关闭风扇温控
    //同时设置所有开关：
    //setAllSwitches(true);     // 打开所有开关
    //setAllSwitches(false);    // 关闭所有开关
    
    // 同时设置所有开关的状态
    void setAllSwitches(bool state) {
      setTypeC32Output(state);
      setTypeC1Output(state);
      setUSBAOutput(state);
      setFan(state);
      setFanTempControl(state);
      setChargeLed(state);
      
      // 发送更新后的状态
      sendSwitchStates();
    }
    
    // ===== WebSocket处理函数 =====
    // 处理WebSocket消息
    void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
      AwsFrameInfo *info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        // 确保数据以null结尾
        data[len] = 0;
        
        // 解析JSON数据
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, (char*)data);
        
        if (!error) {
          // 使用新的推荐语法检查是否存在action字段
          if (!doc["action"].isNull() && doc["action"] == "switch") {
            String switchType = doc["type"].as<String>();
            bool switchState = doc["state"].as<bool>();
            
            // 根据开关类型执行对应操作
            if (switchType == "typeC32") {
              setTypeC32Output(switchState);
            } 
            else if (switchType == "typeC1") {
              setTypeC1Output(switchState);
            }
            else if (switchType == "usbA") {
              setUSBAOutput(switchState);
            } 
            else if (switchType == "fan") {
              setFan(switchState);
            } 
            else if (switchType == "fanTemp") {
              setFanTempControl(switchState);
            } 
            else if (switchType == "chargeLed") {
              setChargeLed(switchState);
            }
            
            // 发送当前所有开关状态给所有客户端
            sendSwitchStates();
          }
        }
      }
    }
    
    // WebSocket事件处理
    void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
                 void *arg, uint8_t *data, size_t len) {
      switch (type) {
        case WS_EVT_CONNECT:
          Serial.printf("WebSocket客户端 #%u 已连接 - %s\n", client->id(), client->remoteIP().toString().c_str());
          // 客户端连接后立即发送当前开关状态给该客户端
          sendSwitchStates(client);
          break;
        case WS_EVT_DISCONNECT:
          Serial.printf("WebSocket客户端 #%u 已断开连接\n", client->id());
          break;
        case WS_EVT_DATA:
          handleWebSocketMessage(arg, data, len);
          break;
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
          break;
      }
    }

    // 初始化WebSocket
void setupWebSocket() {
    ws.onEvent(onEvent);
    server.addHandler(&ws);
  }

  // 初始化Web服务器
void setupWebServer() {
    // 路由处理
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(200, "text/html; charset=UTF-8", index_html);
    });
    
    server.begin();
  }
  
  // ===== 数据发送函数 =====
  // 修改sendSensorData函数添加新数据
  
  void sendSensorData() {
    if (ws.count() > 0) {
      // 创建JSON格式的数据
      JsonDocument doc;
      
      // 添加时间戳
      char timeStr[6];
      unsigned long currentTime = millis();
      sprintf(timeStr, "%02d:%02d", (currentTime / 60000) % 60, (currentTime / 1000) % 60);
      doc["time"] = String(timeStr);
      
      // 添加主电源输入数据
      JsonObject mainPowerData = doc["mainPower"].to<JsonObject>();
      mainPowerData["voltage"] = ina219_get_voltage();
      mainPowerData["current"] = ina219_get_current();
      mainPowerData["power"] = ina219_get_power();
      
      // 为每个Type-C接口图表组生成数据
      for (int i = 1; i <= 3; i++) {
        // 反转group编号：1->3, 2->2, 3->1
        int group = 4 - i;
        String groupKey = "group" + String(i);
        JsonObject groupData = doc[groupKey].to<JsonObject>();
        groupData["voltage"] = ina3221_get_voltage(group);
        groupData["current"] = ina3221_get_current(group);
        groupData["power"] = ina3221_get_power(group);
      }
      
      // 添加USB-A OUT4数据
      JsonObject usbA4Data = doc["usbA4"].to<JsonObject>();
      usbA4Data["voltage"] = adc_get_voltage0();
      
      // 添加USB-A OUT5数据
      JsonObject usbA5Data = doc["usbA5"].to<JsonObject>();
      usbA5Data["voltage"] = adc_get_voltage1();
      
      // 添加系统温度数据
      doc["systemTemp"] = adc_get_temperature();
  
      // 序列化并发送JSON数据
      String jsonString;
      serializeJson(doc, jsonString);
      ws.textAll(jsonString);
    }
  }

  void web_server_run() {
    ws.cleanupClients(); // 清理已断开的客户端
  
    // 定时发送新的随机数据
    unsigned long currentTime = millis();
    if (currentTime - lastTime >= interval) {
      lastTime = currentTime;
      sendSensorData();  // 发送传感器数据
    }
  }