package chenjunfu2.fixlongoverflow.mixin;

import it.unimi.dsi.fastutil.longs.LongSortedSet;
import net.minecraft.world.entity.SectionedEntityCache;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Redirect;

@Mixin(SectionedEntityCache.class)
public class SectionedEntityCacheMixin
{
	@Redirect
	(
		method = "forEachInBox",
		at = @At(value = "INVOKE", target = "Lit/unimi/dsi/fastutil/longs/LongSortedSet;subSet(JJ)Lit/unimi/dsi/fastutil/longs/LongSortedSet;", remap = false)
	)
	private LongSortedSet forEachInBoxMixin(LongSortedSet instance, long l, long r)
	{
		//入参已经被麻将+1了，所以检查是否是最小值已确认是否溢出，
		//但是在这之前还需要确认l>r以防止本身l和r都是最小值的情况
		if(l > r && r == Long.MIN_VALUE)
		{
			return instance.tailSet(l);//r是最大值，变为右区间完全包含
		}
		
		return instance.subSet(l, r);//否则左闭右开
	}
	
	//@ModifyArgs(at = @At(value = "INVOKE", target = "Lit/unimi/dsi/fastutil/longs/LongSortedSet;subSet(JJ)Lit/unimi/dsi/fastutil/longs/LongSortedSet;", remap = false), method = "forEachInBox")
	//private void init(Args args)
	//{
	//	if((Long)args.get(0)>(Long)args.get(1))
	//	{
	//		args.set(1,(Long)args.get(1) - 1L);//+1L刚好溢出，回退1回到最大值，但是边界会被裁剪
	//	}
	//}
}