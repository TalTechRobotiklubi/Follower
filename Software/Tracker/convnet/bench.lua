require 'nn';
require 'image';

torch.setdefaulttensortype('torch.FloatTensor')

function load_data(file)
  local s = torch.load(file)
  s.label:apply(function(x) return x + 1 end)

  setmetatable(s, 
    {__index = function(t, i) 
        return {t.data[i]:view(1, 128, 64), t.label[i]} 
      end
    }
  );

  function s:size() 
    return self.data:size(1)
  end

  return s
end

local netprops = torch.load("cpu.net")
local net = netprops.net
local mean = netprops.mean
local stddev = netprops.stddev

testdata = load_data("test.t7")
testdata.data:add(-mean)
testdata.data:div(stddev)

print(mean, stddev)

timer = torch.Timer()
p = net:forward(testdata.data[9012]:view(1, 128, 64))
print("CPU fwd pass", timer:time().real * 1000)
print(p)
print(p:exp())

local correct = 0
for i = 1, testdata:size() do
  local truth = testdata.label[i]
  local prediction = net:forward(testdata.data[i]:view(1, 128, 64))
  local confidences, indices = torch.sort(prediction, true)
  if truth == indices[1] then
    correct = correct + 1
  end
end 
print(correct, 100 * correct / testdata:size() .. '%')


image.display(testdata.data[9012])
