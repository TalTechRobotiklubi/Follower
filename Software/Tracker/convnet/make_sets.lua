local sqlite3 = require("lsqlite3");
local ffi = require("ffi");

torch.setdefaulttensortype('torch.FloatTensor')

local db = sqlite3.open("out_label_combined.db")

local total = 0
for row in db:rows("select count(*) from labelled") do
  total = row[1]
end

local train_ratio = 0.8
local n_train = math.floor(total * train_ratio)
local n_test = total - n_train

local train = {
  data = torch.Tensor(n_train, 128, 64),
  label = torch.Tensor(n_train)
}

local test = {
  data = torch.Tensor(n_test, 128, 64),
  label = torch.Tensor(n_test)
}

function store_row(o, row, idx)
  local a = ffi.cast("float*", row[1])
  local t = o.data[idx]
  local s = t:storage()
  for i = 0, 8191 do
    s[t:storageOffset() + i] = a[i]
  end
  o.label[idx] = row[2]
end

local idx = 1
local train_idx = 1
local test_idx = 1
for row in db:rows("select depth, human from labelled order by random()") do
  if idx <= n_train then
    store_row(train, row, train_idx)
    train_idx = train_idx + 1
  else
    store_row(test, row, test_idx)
    test_idx = test_idx + 1
  end
  idx = idx + 1
  print("idx: ", idx, " train_idx: ", train_idx, " test_idx: ", test_idx)
end


train.data = train.data:type('torch.FloatTensor')
train.label = train.label:type('torch.FloatTensor')
test.data = test.data:type('torch.FloatTensor')
test.label = test.label:type('torch.FloatTensor')

print(train)
print(test)

torch.save("train.t7", train)
torch.save("test.t7", test)
