--!
--! \file FDdialog.lua
--! \brief This file contains the Lua part of the dialog engine and its related classes.
--!

-- Avoid doxygen to complain about missing namespace definition
--! \namespace Lua

FDdialog = {}

----------------------------------------
--! \class Lua::FDdialog::Stack
--! \brief Stack class
----------------------------------------

FDdialog.Stack = {
	--! \privatesection
    head = 0 --!< \brief Stack head index, '0' when empty     \memberof Lua::FDdialog::Stack 
	--! \publicsection
}

--! \fn bool is_empty()
--!
--! \brief Check if stack is empty
--!
--! \return true is stack is empty
--!
--! \memberof Lua::FDdialog::Stack

function FDdialog.Stack.is_empty(self)
	return (self.head == 0)
end

--! \fn void push(Element obj)
--!
--! \brief Push an element on top of the stack
--!
--! \param obj Element to push on stack
--!
--! \memberof Lua::FDdialog::Stack

function FDdialog.Stack.push(self, obj)
	self.head = self.head + 1
	self[self.head] = obj
end

--! \fn Element pop()
--!
--! \brief Pop an element from the top of the stack
--!
--! \return Poped element
--!
--! \memberof Lua::FDdialog::Stack

function FDdialog.Stack.pop(self)
	if self:is_empty() then return nil end
	local value = self[self.head]
	self[self.head] = nil
	self.head = self.head - 1
	return value
end

--! \fn Element top()
--!
--! \brief Return the top element of the stack, without changing the stack
--!
--! \return Top element or nil if stack is empty
--!
--! \memberof Lua::FDdialog::Stack

function FDdialog.Stack.top(self)
	if self:is_empty() then return nil end
	return self[self.head]
end

--! \fn Stack new()
--!
--! \brief Create a new stack instance
--!
--! \return Created stack
--!
--! \memberof Lua::FDdialog::Stack

function FDdialog.Stack.new()
	local instance = {}
	setmetatable(instance, {__index = FDdialog.Stack})
	return instance
end

----------------------------------------
--! \class Lua::FDdialog::Node
--! \brief Node class
----------------------------------------

FDdialog.Node = {
	--! \privatesection
	id = "NOID",		            --!< \brief String ID                                                                  \memberof Lua::FDdialog::Node
	enabled = false,	            --!< \brief Enable flag : If true, the node is returned by Dialog.get_options()        \memberof Lua::FDdialog::Node
	text = "NOTEXT",	            --!< \brief Text displayed in the Tux options list - can be a string or a function     \memberof Lua::FDdialog::Node
	topic = "",			            --!< \brief Used to group nodes into specific sub-parts of a dialog                    \memberof Lua::FDdialog::Node
	code = function() return end	--!< \brief Script to run when the node is activated                                   \memberof Lua::FDdialog::Node 
	--! \publicsection
}

--! \fn string get_text()
--!
--! \brief Get the text of the node
--!
--! \return If 'text' is a string, returns its content. If 'text' is a function, returns the return value of the function
--!
--! \memberof Lua::FDdialog::Node

function FDdialog.Node.get_text(self)
	if (type(self.text) == "string") then
		return self.text
	elseif (type(self.text) == "function") then
		return self.text()
	end
end

--! \fn Node new(table arg)
--!
--! \brief Create a new node instance
--!
--! \param  arg If 'arg' is not nil, the node instance is filled with the content of the 'arg' table
--! \return     The created node, or (nil, error_msg) if the check of the mandatory fields fails
--!
--! \memberof Lua::FDdialog::Node

function FDdialog.Node.new(arg)
	local instance = arg or {}
	-- Check mandatory fields, before to set the metatable (and so before to
	-- inherit from default values)
	if (not instance.id or instance.id == "") then
		error(FDutils.text.red("Missing or empty node's id") .. "\n" .. FDutils.dump.table(instance, "Node content"), 0)
	end
	if (not instance.code or type(instance.code) ~= "function") then
		error(FDutils.text.red("Missing or non-funtion node's code") .. "\n" .. FDutils.dump.table(instance, "Node content"), 0)
	end
	-- Inherit from Node "interface"
	setmetatable(instance, {__index = FDdialog.Node})
	-- Node's id has to be a string
	instance.id = tostring(instance.id)

	return instance
end

----------------------------------------
--! \class Lua::FDdialog::Dialog
--! \brief Dialog class
----------------------------------------

FDdialog.Dialog = {
	--! \privatesection
	name = "",          --!< \brief Dialog's name                                                   \memberof Lua::FDdialog::Dialog
	nodes = nil,		--!< \brief List of dialog's nodes                                          \memberof Lua::FDdialog::Dialog
	topics = nil,		--!< \brief Stack of topics (see Node)                                      \memberof Lua::FDdialog::Dialog
	next_node = nil		--!< \brief Next node to run after the end of the current node's script     \memberof Lua::FDdialog::Dialog
	--! \publicsection
}

--! \fn table get_options()
--!
--! \brief Get the list of enabled nodes text
--!
--! \return Array of node indexes + node texts (array[2*i] = node's index, array[2*i+1] = node's text)
--!
--! \memberof Lua::FDdialog::Dialog

function FDdialog.Dialog.get_options(self)
	local current_topic = self.topics:top() or ""
	local options = {}
	for i,node in ipairs(self.nodes) do
		if (node.enabled and node.topic == current_topic) then
			options[#options + 1] = i
			options[#options + 1] = node:get_text()
		end
	end
	return options
end

--! \fn pair find_nodes(string nodeid)
--!
--! \brief Return a node given its ID
--!
--! \param  nodeid ID of the searched node
--! \return        Pair of (index, node) or (nil, nil) if not found
--!
--! \memberof Lua::FDdialog::Dialog

function FDdialog.Dialog.find_node(self, nodeid)
	for i,node in ipairs(self.nodes) do
		if (node.id == nodeid) then
			return i, node
		end
	end
	return nil, nil
end

--! \fn void foreach_node(string{} nodes, void (*func)(string))
--!
--! \brief Execute a function on a list of nodes (internal function)
--!
--! \param nodes List of node's ids
--! \param func  Function to execute
--!
--! \memberof Lua::FDdialog::Dialog

function FDdialog.Dialog.foreach_node(self, nodes, func)
	for _,nodeid in ipairs(nodes) do
		local i, node = self:find_node(nodeid)
		if (node) then
			func(node)
		end
	end
end

--! \fn void enable_nodes(string{} nodes)
--!
--! \brief Enable a list of nodes
--!
--! \param nodes List of node's ids
--!
--! \memberof Lua::FDdialog::Dialog

function FDdialog.Dialog.enable_nodes(self, nodes)
	self:foreach_node(nodes, function(node) node.enabled = true end)
end

--! \fn void disable_nodes(string{} nodes)
--!
--! \brief Disable a list of nodes
--!
--! \param nodes List of node's ids
--!
--! \memberof Lua::FDdialog::Dialog

function FDdialog.Dialog.disable_nodes(self, nodes)
	self:foreach_node(nodes, function(node) node.enabled = false end)
end

--! \fn Dialog new(string name, string filename)
--!
--! \brief Create a new dialog instance from a dialog description
--!
--! \param  name     Name of the dialog
--! \param  filename Path of the file containing the dialog's description
--! \return          Created dialog or throw an error
--!
--! \memberof Lua::FDdialog::Dialog

function FDdialog.Dialog.new(name, filename)
	local new_dialog = filename and dofile(filename) or {}
	setmetatable(new_dialog, {__index = FDdialog.Dialog})

	local function _insert_node(node_def, with_topic)
		-- Create a new node instance
		local rtn,data = pcall(FDdialog.Node.new, node_def)
		if (not rtn) then
			error("Error while loading " .. filename .. "\n" .. data, 0)
		end
		local node = data
		if (with_topic) then
			node.topic = with_topic
		end
		-- Check if a node with same id is already registered
		local registered = new_dialog:find_node(node.id)
		if (registered) then
			error("Error while loading " .. filename .. "\n" ..
			      FDutils.text.red("An other node with id \"" .. node.id .. "\" is already registered"), 0)
		end
		-- Insert node
		table.insert(new_dialog.nodes, node)
	end

	new_dialog.name = name
	new_dialog.nodes = {}
	new_dialog.topics = FDdialog.Stack.new()

	--
	-- Extract nodes from dialog array, create Node instances and add them to 
	-- new_dialog.nodes
	--
	for node_number, node_def in ipairs(new_dialog) do
		if (not node_def.subnodes) then
			_insert_node(node_def)
		else
			-- If there is a subnodes key in the parsed node, then the subnodes
			-- are extracted and added to the dialog
			local subnodes = nil
			if (type(node_def.subnodes) == "function") then
				subnodes = node_def.subnodes(new_dialog)
			else
				subnodes = node_def.subnodes
			end
			for _,subnode_def in ipairs(subnodes) do
				_insert_node(subnode_def, node_def.topic or "")
			end
			subnodes = nil
		end
		new_dialog[node_number] = nil
	end

	return new_dialog
end

--! \fn int validate()
--!
--! \brief Check a dialog description
--!
--! \return 1 if test passed, 0 if failed
--!
--! \memberof Lua::FDdialog::Dialog

function FDdialog.Dialog.validate(self)
	local valid = 1

	local function _try(node, script)
		if (script) then
			local rtn,err = pcall(script, node, self)
			if (not rtn) then
				print("\n" .. FDutils.text.red("Execution error: " .. err))
				valid = 0
				return false
			end
		end
		return true
	end

	--
	-- Check specific nodes
	--
	if (self.FirstTime) then
		io.write("  FirstTime")
		_try(nil, self.FirstTime)
	end

	if (self.EveryTime) then
		io.write("  EveryTime")
		_try(nil, self.EveryTime)
	end

	--
	-- Check generic nodes
	--
	local count = 5
	for index,node in ipairs(self.nodes) do
		if (count == 5) then
			io.write("\n")
			count = 0;
		end
		io.write("  |\"" .. node.id .. "\"|");
		if (not _try(node, node.code)) then
			count = 0
		else
			count = count + 1
		end
	end

	io.write("\n")
	return valid
end

------------
--! \class Lua::FDdialog
--! \brief Public API of the Lua part of the dialog engine.
------------

--! \privatesection
FDdialog.dialogs_dir = "../dialogs"     --!< \brief Directory containing the dialogs     \memberof Lua::FDdialog
FDdialog.stack = FDdialog.Stack.new()   --!< \brief Dialogs stack                        \memberof Lua::FDdialog
--! \publicsection

--! \fn void set_dialog_dir(string dir)
--!
--! \brief Set dialogs' directory
--!
--! \param dir Directory containing the dialogs
--!
--! \memberof Lua::FDdialog

function FDdialog.set_dialog_dir(dir)
	FDdialog.dialogs_dir = dir
end

--! \fn string{} get_options()
--!
--! \brief Return a table containing the current dialog options of Tux
--!
--! \return Table containing a mixed list of option indexes and option texts
--!
--! \memberof Lua::FDdialog

function FDdialog.get_options()
	local current_dialog = FDdialog.stack:top()
	return current_dialog and current_dialog:get_options()
end

--! \fn void run_init()
--!
--! \brief Run the FirtTime script of the current dialog
--!
--! \memberof Lua::FDdialog

function FDdialog.run_init()
	local current_dialog = FDdialog.stack:top()
	local script = current_dialog and current_dialog.FirstTime
	current_dialog.next_node = nil
	if (not script) then
		return
	end
	script(current_dialog)
	return current_dialog.next_node and FDdialog.run_node(current_dialog.next_node)
end

--! \fn void run_startup()
--!
--! \brief Run the EveryTime script of the current dialog
--!
--! \memberof Lua::FDdialog

function FDdialog.run_startup()
	local current_dialog = FDdialog.stack:top()
	local script = current_dialog and current_dialog.EveryTime
	current_dialog.next_node = nil
	if (not script) then
		return
	end
	script(current_dialog)
	return current_dialog.next_node and FDdialog.run_node(current_dialog.next_node)
end

--! \fn void run_node(string node_idx)
--!
--! \brief Run the script of a given node of the current dialog
--!
--! \param node_idx Index of the node to use
--!
--! \memberof Lua::FDdialog

function FDdialog.run_node(node_idx)
	local current_dialog = FDdialog.stack:top()
	local node = current_dialog and current_dialog.nodes[node_idx]
	current_dialog.next_node = nil
	if (not node) then
		return
	end
	if (node.enabled) then
		tux_says(node:get_text(), "NO_WAIT")
	end
	node:code(current_dialog)
	return current_dialog.next_node and FDdialog.run_node(current_dialog.next_node)
end

--! \fn void enable_nodes(...)
--!
--! \brief Enable a list of nodes in the current dialog
--!
--! \param ... List of nodes to enable
--!
--! \memberof Lua::FDdialog

function FDdialog.enable_nodes(...)
	local current_dialog = FDdialog.stack:top()
	return current_dialog and current_dialog:enable_nodes({...})
end

--! \fn void disable_nodes(...)
--!
--! \brief Disable a list of nodes in the current dialog
--!
--! \param ... List of nodes to disable
--!
--! \memberof Lua::FDdialog

function FDdialog.disable_nodes(...)
	local current_dialog = FDdialog.stack:top()
	return current_dialog and current_dialog:disable_nodes({...})
end

--! \fn void push_topic(string topic)
--!
--! \brief Push a topic key of the current topic stack
--!
--! \param topic Topic key to push
--!
--! \memberof Lua::FDdialog

function FDdialog.push_topic(topic)
	local current_dialog = FDdialog.stack:top()
	return current_dialog and current_dialog.topics:push(topic)
end

--! \fn string pop_topic()
--!
--! \brief Pop a topic key of the current topic stack and return it
--!
--! \return The topic key that was on top of the topic stack
--!
--! \memberof Lua::FDdialog

function FDdialog.pop_topic()
	local current_dialog = FDdialog.stack:top()
	return current_dialog and current_dialog.topics:pop()
end

--! \fn void push_dialog(string dialog_name, table enabled_nodes)
--!
--! \brief Push a dialog on the dialog stack
--!
--! \param dialog_name   Name of the dialog to push (without the .lua suffix)
--! \param enabled_nodes List of nodes to enable
--!
--! \memberof Lua::FDdialog

function FDdialog.push_dialog(dialog_name, enabled_nodes)
	local new_dialog = {}
	local dialog_file = find_file(dialog_name..".lua", FDdialog.dialogs_dir)
	if (not dialog_file) then
		error("Dialog file not found: " .. dialog_name .. ".lua\n", 0)
	end
	new_dialog = FDdialog.Dialog.new(dialog_name, dialog_file)
	new_dialog:enable_nodes(enabled_nodes)
	FDdialog.stack:push(new_dialog)
end

--! \fn string{] pop_dialog()
--!
--! \brief Pop the current dialog from the dialog stack and return the list of enabled nodes
--!
--! \return Lst of enabled nodes
--!
--! \memberof Lua::FDdialog

function FDdialog.pop_dialog()
	if FDdialog.stack:is_empty() then
		print("ERROR: stack is empty")
		return nil
	end

	local current_dialog = FDdialog.stack:top()

	local enabled_nodes = {}
	for i,node in ipairs(current_dialog.nodes) do
		if (node.enabled) then
			enabled_nodes[#enabled_nodes + 1] = node.id
		end
	end

	FDdialog.stack:pop()

	return enabled_nodes
end

--! \fn bool validate_dialog(string dialog_name)
--!
--! \brief Run validation test on the current top dialog
--!
--! \param  dialog_name Name of the dialog to push (without the .lua suffix)
--! \return             1 if the dialog is valid, or 0
--!
--! \memberof Lua::FDdialog

function FDdialog.validate_dialog(dialog_name)
	local rtn,err = pcall(FDdialog.push_dialog, dialog_name, {})
	if (not rtn) then
		print(err)
		return 0
	end
	local current_dialog = FDdialog.stack:top()
	local valid = current_dialog:validate()
	FDdialog.pop_dialog()
	return valid
end

--! \fn void include(string subdialog_name)
--!
--! \brief Load a subdialog file. Used to include it into the nodes list of the current dialog
--!
--! \param subdialog_name Name of the subdialog to load (without the .lua suffix)
--!
--! \memberof Lua::FDdialog

function FDdialog.include(subdialog_name)
	return dofile(find_file(subdialog_name..".lua", FDdialog.dialogs_dir))
end

--! \fn void next_node(string nodename)
--!
--! \brief Register a node to be run after the current node script ends
--!
--! \param nodename The node to run next
--!
--! \memberof Lua::FDdialog

function FDdialog.next_node(nodename)
	local current_dialog = FDdialog.stack:top()
	local idx, node = current_dialog:find_node(nodename)
	if (not idx) then
		print("next() called on unknown node: ", nodename)
	else
		current_dialog.next_node = idx
	end
end

--[[ Global functions to handle the dialog engine ]]--

next = FDdialog.next_node
show = FDdialog.enable_nodes
hide = FDdialog.disable_nodes
push_topic = FDdialog.push_topic
pop_topic = FDdialog.pop_topic
include = FDdialog.include

return FDdialog