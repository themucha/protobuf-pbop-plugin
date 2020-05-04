#include <stdio.h>

#include "calculator.pipe.pb.h"
#include "libProtobufPipePlugin/Server.h"

using namespace libProtobufPipePlugin;
using namespace calculus;

static const char * kPipeName = "\\\\.\\pipe\\mynamedpipe";

class CalculatorServiceImpl : public CalculatorService::ServerStub
{
public:
  CalculatorServiceImpl() {}
  virtual ~CalculatorServiceImpl() {}

  Status Add(const AddRequest & request, AddResponse & response)
  {
    const int & left = request.left();
    const int & right = request.right();
    int result = left + right;
    response.set_value(result);
    return Status::OK;
  }

};

int RunClient()
{
  printf("Launching client...\n");

  Connection * connection = new PipeConnection();
  Status status = connection->Connect(kPipeName);
  if (!status.Success())
  {
    printf("Error in main(): %d, %s\n", status.GetCode(), status.GetMessage().c_str());
    return status.GetCode();
  }

  CalculatorService::Client client(connection);

  AddRequest request;
  AddResponse response;

  int left = 15;
  int right = 3;

  request.set_left(left);
  request.set_right(right);

  printf("Calling Add(%d,%d)\n", left, right);
  status = client.Add(request, response);
  if (!status.Success())
  {
    printf("Error in main(): %d, %s\n", status.GetCode(), status.GetMessage().c_str());
    return status.GetCode();
  }

  printf("The result is %d\n", response.value());

  return 0;
}

int RunServer()
{
  printf("Launching server...\n");

  CalculatorServiceImpl * impl = new CalculatorServiceImpl();

  Server server;
  server.RegisterService(impl);
  Status status = server.Run(kPipeName);
  if (!status.Success())
  {
    printf("Error in main(): %d, %s\n", status.GetCode(), status.GetMessage().c_str());
    return status.GetCode();
  }
  return 0;
}

int main(int argc, char* argv[])
{
  if (argc == 2 && std::string(argv[1]) == "client")
  {
    return RunClient();
  }
  else if (argc == 2 && std::string(argv[1]) == "server")
  {
    return RunServer();
  }
  else
  {
    printf("Error. Launch program with 'client' or 'server' argument\n");
    return 1;
  }
}
