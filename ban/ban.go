package ban

import (
	"cargeo/context"
)

// Bans used for ban ip
type Bans struct {
	banChain map[FuncBan]bool
}

// FuncBan used for ban ip
// user can defin own BanFunc
// if func return true, this ip would banned
type FuncBan func(c *context.Context) bool

func NewBans() *Bans {
	return &Bans{
		banChain: make(map[FuncBan]bool),
	}
}
func (bs Bans) AddFuncBan(fb FuncBan) bool {
	if bs.banChain[fb] {
		return false
	}
	bs.banChain[fb] = true
	return true
}
