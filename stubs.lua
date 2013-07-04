function irctolower(channel)
  if string.len(channel) == 0 or channel == "#" then
    return channel;
  end
  channel = string.gsub(channel, "%[", "{")
  channel = string.gsub(channel, "%]", "}")
  channel = string.gsub(channel, "\\", "|")
  return string.lower(channel)
end

nickpusher = {}

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
  local i = 1
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

function mode_iter(t)
  local n = 1
  local max = table.getn(t)
  return function()
    local o
    if n <= max then
      o = {plsmns = t[n], mode = t[n+1], target = t[n+2]}
    end
    n = n + 3
    return o
  end
end

function irc_localovmode(bot, chan, modes)
  local modestr, targets, lastplsmns, modecount = "", "", nil, 0

  for m in mode_iter(modes) do
    if lastplsmns == nil or m.plsmns ~= lastplsmns then
      if m.plsmns then
        modestr = modestr .. "+"
      else
        modestr = modestr .. "-"
      end
      lastplsmns = m.plsmns
    end
    modestr = modestr .. m.mode
    targets = targets .. " " .. m.target
    modecount = modecount + 1
    if modecount == 6 then
      irc_localmode(bot, chan, modestr .. targets)
      modestr, targets, lastplsmns, modecount = "", "", nil, 0
    end
  end
  if modestr ~= "" then
    irc_localmode(bot, chan, modestr .. targets)
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
  local future = irc_now() + secs
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
  local now = irc_now()
  local newtasks = {}
  for time, funcs in pairs(self.tasks) do
    if time <= now then
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
