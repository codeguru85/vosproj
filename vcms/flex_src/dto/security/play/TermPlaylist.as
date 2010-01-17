package dto.security.play
{
	import dto.security.client.TermInfo;
	import mx.collections.ArrayCollection;
	[Bindable]
	[RemoteClass(alias="com.broadengate.vcms.entity.TermPlaylist")]
	public class TermPlaylist
	{
		public var termListId:int;
		public var playlist:Playlist;
		public var termInfo:TermInfo;
		public var createTime:Date;
		public function TermPlaylist()
		{
		}

	}
}