#define K1        2
#define K1_MASK   0x01 
#define K2        3
#define K2_MASK   0x02

byte trg = 0, cont = 0;
byte buf[32], len;

//求校验和
byte getSum(byte *data, byte len) {
  byte i, sum = 0;
  for (i = 0; i < len; i++) {
    sum += data[i];
  }
  return sum;
}

//红外内码学习
byte IrStudy(byte *data, byte group) {
  byte *offset = data, cs;
  //帧头
  *offset++ = 0x68;
  //帧长度
  *offset++ = 0x08;
  *offset++ = 0x00;
  //模块地址
  *offset++ = 0xff;
  //功能码
  *offset++ = 0x10;
  //内码索引号，代表第几组
  *offset++ = group;
  cs = getSum(&data[3], offset - data - 3);
  *offset++ = cs;
  *offset++ = 0x16;
  return offset - data; 
}

//红外内码发送
byte IrSend(byte *data, byte group) {
  byte *offset = data, cs;
  //帧头
  *offset++ = 0x68;
  //帧长度
  *offset++ = 0x08;
  *offset++ = 0x00;
  //模块地址
  *offset++ = 0xff;
  //功能码
  *offset++ = 0x12;
  //内码索引号，代表第几组
  *offset++ = group;
  cs = getSum(&data[3], offset - data - 3);
  *offset++ = cs;
  *offset++ = 0x16;
  return offset - data; 
}

//按键状态回读
void keyRead() {
  byte data = 0x00;
  if (digitalRead(K1) == 0) {
    data |= K1_MASK;   
  }
  if (digitalRead(K2) == 0) {
    data |= K2_MASK;  
  }
  trg = data & (data ^ cont);
  cont = data;
}

//按键按下检测处理
void keyHandle() {
  if (trg == 0x00) {
      return;
  }
  //按键1按下，开始红外内码学习
  if (trg & K1_MASK) {
    len = IrStudy(buf, 0);
    Serial.write(buf, len);
  }
  //按键2按下，开始红外内码发射
  if (trg & K2_MASK) {
    len = IrSend(buf, 0);
    Serial.write(buf, len);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(K1, INPUT_PULLUP);
  pinMode(K2, INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
  keyRead();
  keyHandle();
  delay(20);
}
