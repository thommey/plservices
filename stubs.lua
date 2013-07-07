os = {}
nickpusher = {}

function os.time()
  return os_time()
end

function nickpusher.nick(user)
  return user.nick
end

function nickpusher.numeric(user)
  return user.numeric
end

function nickpusher.accountid(user)
  return user.accountid
end

function channelusers_iter(chan, dataselectors)
  local i = 0
  local chanusers = irc_channeluserlist(chan)
  local n = table.getn(chanusers)
  return function()
    local o = {}
    i = i + 1
    if i <= n then
      for j, dataselector in ipairs(dataselectors) do
        o[j] = dataselector(chanusers[i])
      end
      return o
    end
  end
end

-- TODO: database management
function basepath()
  return ""
end

function loadtable(path)
  return nil
end

function savetable(path)
  return nil
end

Scheduler = {}
Scheduler.__index = Scheduler
Schedulers = {}

function Scheduler.new()
  local sched = setmetatable({tasks = {}}, Scheduler)
  table.insert(Schedulers, sched)
  return sched
end

setmetatable(Scheduler, { __call = function(_, ...) return Scheduler.new(...) end })

function Scheduler:add(secs, func, ...)
  local future = os.time() + secs
  local f = { func = func, args = { ... } }

  if not self.tasks[future] then
    self.tasks[future] = {}
  end

  table.insert(self.tasks[future], f)

  return f
end

function Scheduler:remove(call)
  local found = 0
  for time, funcs in pairs(self.tasks) do
    for _, f in ipairs(funcs) do
      if f == call then
        found = 1
        table.remove(self.tasks[time], i)
      end
    end
  end
end

function Scheduler:check()
  local time = os.time()
  local newtasks = {}
  for time, funcs in pairs(self.tasks) do
    if time <= time then
      for _, f in ipairs(funcs) do
        f.func(unpack(f.args))
      end
    else
      newtasks[time] = funcs
    end
  end
  self.tasks = newtasks
end

function Scheduler.allcheck()
  for _, sched in ipairs(Schedulers) do
    sched:check()
  end
end

function ontick2()
  Scheduler.allcheck()
end
