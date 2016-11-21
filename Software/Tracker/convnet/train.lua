require 'nn';
require 'cunn';
require 'paths';
require 'pack';

torch.setdefaulttensortype('torch.FloatTensor')

cmd = torch.CmdLine()
cmd:text("Options")
cmd:option("--file", "", "t7 file")
params = cmd:parse(arg)

function load_data(file)
  local s = torch.load(file)
  s.label:apply(function(x) return x + 1 end)
  s.data = s.data:cuda()
  s.label = s.label:cuda()

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

traindata = load_data(params.file)
if traindata == nil then
  print("Unable to load training set")
end

mean = traindata.data:mean()
traindata.data:add(-mean)

stddev = traindata.data:std()
traindata.data:div(stddev)

net = nn.Sequential()
-- out: 5x60x124
net:add(nn.SpatialConvolution(1, 6, 5, 5))
net:add(nn.ReLU())
-- 5x30x62
net:add(nn.SpatialMaxPooling(2,2,2,2))
net:add(nn.SpatialConvolution(6, 16, 5, 5))
net:add(nn.ReLU())
net:add(nn.SpatialMaxPooling(2,2,2,2))
net:add(nn.View(16*29*13))
net:add(nn.Linear(16*29*13, 120))
net:add(nn.ReLU())
net:add(nn.Linear(120, 84))
net:add(nn.ReLU())
net:add(nn.Linear(84, 2))
net:add(nn.LogSoftMax())
net = net:cuda()

print('\n' .. net:__tostring());

criterion = nn.ClassNLLCriterion()
criterion = criterion:cuda()
trainer = nn.StochasticGradient(net, criterion)
trainer.learningRate = 0.001
trainer.maxIteration = 8

trainer:train(traindata)

torch.save("gpu.net", {net=net, mean=mean, stddev=stddev})
net:float()
torch.save("cpu.net", {net=net, mean=mean, stddev=stddev})
