/**
@Author Michael Monaghan
Facilitates comunication using the RFM95 series of chips using the spidev
interface.
*/
#define PACKET_LENGTH 1

using namespace std;

class Radio {
private:
  int fd;
  bool error;
  void setReg(unsigned char reg, unsigned char value);
  unsigned char getReg(unsigned char reg);
  void readFIFO(void *data, unsigned char length);
  void writeFIFO(const void *data, unsigned char length);
public:
  Radio();
  void init();
  void transmit(const void *data, int length);
  void startReceiving();
  void receive(void *data);
  void standby();
  void transmit(const void *data);
};
